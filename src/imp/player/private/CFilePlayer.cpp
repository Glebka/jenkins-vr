/*************************************************************************
 * jenkins-vr
 *************************************************************************
 * @file    CFilePlayer.cpp
 * @date    12.06.16
 * @author  Hlieb Romanov
 * @brief   File player class implementation based on GStreamer
 ************************************************************************/
#include <gst/gst.h>
#include <glib.h>
#include <boost/format.hpp>
#include <boost/thread.hpp>
#include <boost/chrono.hpp>

#include "imp/player/CFilePlayer.hpp"
#include "imp/logger/CLogger.hpp"

using namespace api::player;

static const guint64 MAX_WAIT_TIMEOUT = 5 * GST_SECOND;
static const int MAX_PLAYBACK_DURATION_SEC = 30;

static const char* PIPELINE_SPEC = "filesrc name=fsrc ! wavparse ! audioconvert ! autoaudiosink";
static const char* FILESRC_NAME = "fsrc";
static const char* FILESRC_LOCATION_PARAM = "location";

static const char* PIPELINE_ERROR_MSG = "Can't create the player pipeline, GStreamer returns NULL";
static const char* FILESRC_ERROR_MSG = "Can't create the filesrc element, GStreamer returns NULL";
static const char* PLAYER_ERROR_MSG = "Player may be in inconsistent state. Aborting.";
static const char* PARSE_ERROR_MSG = "GStreamer error (%1%): %2%";

static void THROW_FATAL( const std::string&  message )
{
   CLogger::fatal() << message;
   throw std::exception( message.c_str() );
}

class CFilePlayer::CGstPlayerPipeline: boost::noncopyable
{
public:
   CGstPlayerPipeline( void )
      : mPipeline( NULL )
      , mFileSrc( NULL )
      , mLoop( NULL ) 
      , mIsEos( false )
   {
      GError* error = NULL;
      GstElement* pipeline = gst_parse_launch( PIPELINE_SPEC, &error );
      if ( error )
      {
         CLogger::warning() << str( boost::format( PARSE_ERROR_MSG ) % error->code % error->message );
         g_error_free( error );
      }
      if ( pipeline == NULL )
      {
         THROW_FATAL( PIPELINE_ERROR_MSG );
      }
      mPipeline = GST_PIPELINE( pipeline );
      mFileSrc = gst_bin_get_by_name( GST_BIN( pipeline ), FILESRC_NAME );
      if ( mFileSrc == NULL )
      {
         THROW_FATAL( FILESRC_ERROR_MSG );
      }
      mLoop = g_main_loop_new( NULL, FALSE );
      GstBus* bus = gst_pipeline_get_bus( mPipeline );
      guint bus_watch_id = gst_bus_add_watch( bus, bus_call, self() );
      gst_object_unref (bus);
      boost::thread mainLoopRunner = boost::thread( [this]() {
         g_main_loop_run( mLoop );
      } );
      mainLoopRunner.detach();
   }

   CGstPlayerPipeline* self( void )
   {
      return this;
   }

   bool startPlaying( const std::string& filename )
   {
      bool result = false;
      if ( !isPlaying() )
      {
         {
            boost::lock_guard<boost::mutex> lock( mConditionGuard );
            mIsEos = false;
         }
         g_object_set( G_OBJECT( mFileSrc ), FILESRC_LOCATION_PARAM, filename.c_str(), NULL );
         gst_element_set_state( GST_ELEMENT( mPipeline ), GST_STATE_PLAYING );
         result = isPlaying();
      }
      return result;
   }

   void stopPlaying( void )
   {
      if ( isPlaying() )
      {
         gst_element_set_state( GST_ELEMENT( mPipeline ), GST_STATE_NULL );
         isPlaying();
         {
            boost::lock_guard<boost::mutex> lock( mConditionGuard );
            mIsEos = true;
         }
         mCondition.notify_all();
      }
   }

   bool waitForCompletion( void )
   {
      boost::unique_lock<boost::mutex> lock( mConditionGuard );
      return mCondition.wait_for( lock, 
         boost::chrono::seconds( MAX_PLAYBACK_DURATION_SEC ), 
         boost::bind( &CGstPlayerPipeline::isEos, this ) );
   }

   bool isPlaying( void )
   {
      bool playing = false;
      GstState state;
      GstState pending;
      GstStateChangeReturn result = gst_element_get_state( GST_ELEMENT( mPipeline ), &state, &pending, MAX_WAIT_TIMEOUT );
      if ( result == GST_STATE_CHANGE_FAILURE )
      {
         THROW_FATAL( PLAYER_ERROR_MSG );
      }
      if ( state == GST_STATE_PLAYING )
      {
         playing = true;
      }
      else
      {
         if ( state == GST_STATE_NULL )
         {
            playing = false;
         }
         else
         {
            THROW_FATAL( PLAYER_ERROR_MSG );
         }
      }
      return playing;
   }

   ~CGstPlayerPipeline( void )
   {
      if ( isPlaying() )
      {
         stopPlaying();
      }
      gst_object_unref( GST_OBJECT( mPipeline ) );
      g_main_loop_quit( mLoop );
   }

private:
   bool isEos( void )
   {
      return mIsEos;
   }

   void onBusCall( GstBus* bus, GstMessage* msg )
   {
      switch ( GST_MESSAGE_TYPE( msg ) )
      {
      case GST_MESSAGE_EOS:
         stopPlaying();
         break;
      case GST_MESSAGE_ERROR: 
      {
         gchar* debug;
         GError* error;

         gst_message_parse_error(msg, &error, &debug);
         g_free (debug);
         THROW_FATAL( str( boost::format( PARSE_ERROR_MSG ) % error->code % error->message ) );
         g_error_free (error);
         break;
      }
      default:
         break;
      }
   }

   static gboolean bus_call( GstBus* bus, GstMessage* msg, gpointer data )
   {
      CGstPlayerPipeline* player = reinterpret_cast<CGstPlayerPipeline*>( data );
      player->onBusCall( bus, msg );
      return TRUE;
   }

private:
   GstPipeline* mPipeline;
   GstElement* mFileSrc;
   GMainLoop* mLoop;
   bool mIsEos;
   boost::thread mLoopThread;
   boost::mutex mConditionGuard;
   boost::condition_variable mCondition;
};

CFilePlayer::CFilePlayer( void )
   : mPlayerPipeline( new CGstPlayerPipeline() )
{

}

api::player::FilePlayerPtr CFilePlayer::create( void )
{
   return FilePlayerPtr( new CFilePlayer() );
}

CFilePlayer::~CFilePlayer( void )
{

}

bool CFilePlayer::startPlaying( const std::string& fileName )
{
   bool result = false;
   std::string file( fileName );
   
   result = mPlayerPipeline->startPlaying( fileName );
   if ( result )
   {
      boost::thread notifier = boost::thread( [this, file]() {
         mPlayerPipeline->waitForCompletion();
         mStopPlaying( StopPlayingData( file ) );
      } );
      notifier.detach();
   }
   mStartPlaying( StartPlayingData( fileName, result ) );
   return result;
}

void CFilePlayer::stopPlaying( void )
{
   mPlayerPipeline->stopPlaying();
}

bool CFilePlayer::playFile( const std::string& fileName )
{
   bool result = startPlaying( fileName );
   if ( result )
   {
      result = mPlayerPipeline->waitForCompletion();
   }
   return result;
}

bool CFilePlayer::isPlaying( void ) const
{
   return mPlayerPipeline->isPlaying();
}

signals::connection CFilePlayer::onStartPlaying( const api::player::StartPlayingSignal_t::slot_type& slot )
{
   return mStartPlaying.connect( slot );
}

signals::connection CFilePlayer::onStopPlaying( const api::player::StopPlayingSignal_t::slot_type& slot )
{
   return mStopPlaying.connect( slot );
}