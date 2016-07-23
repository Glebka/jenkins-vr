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
#include <boost/chrono.hpp>

#include "imp/player/CFilePlayer.hpp"
#include "imp/logger/CLogger.hpp"
#include "imp/gstreamer/CGstPipeline.hpp"

using namespace api::player;

GST_DEBUG_CATEGORY_STATIC (player_debug);

static const guint64 MAX_WAIT_TIMEOUT = 5 * GST_SECOND;
static const int MAX_PLAYBACK_DURATION_SEC = 10;

static const char* PIPELINE_SPEC = "filesrc name=fsrc ! wavparse ! audioconvert ! audioresample ! autoaudiosink name=sink";
static const char* FILESRC_NAME = "fsrc";
static const char* FILESRC_LOCATION_PARAM = "location";

static const char* PIPELINE_ERROR_MSG = "Can't create the player pipeline, GStreamer returns NULL";
static const char* FILESRC_ERROR_MSG = "Can't create the filesrc element, GStreamer returns NULL";
static const char* PLAYER_ERROR_MSG = "Player may be in inconsistent state. Aborting.";
static const char* PARSE_ERROR_MSG = "GStreamer error (%1%): %2%";

typedef boost::shared_ptr<GMainLoop> MainLoopPtr;

static void finalizeMainLoop( GMainLoop* loop )
{
    g_main_loop_quit( loop );
    g_main_loop_unref( loop );
}

/**
 * TODO: make common hpp and cpp files and move utility functions to it.
 */
static void THROW_FATAL( const std::string&  message )
{
   CLogger::fatal() << message;
   throw std::runtime_error( message );
}

/**
 * TODO: refactor here, move to separate file
 */
class CFilePlayer::CGstPlayerPipeline: boost::noncopyable
{
public:
   CGstPlayerPipeline( void )
      : mPipeline( CGstPipeline( PIPELINE_SPEC ) )
      , mFileSrc( mPipeline.getElementByName( FILESRC_NAME ) )
      , mSink( mPipeline.getElementByName( "sink" ) )
      , mSinkPad( mSink.getSinkPad() )
      , mLoop( NULL ) 
      , mIsEos( false )
   {
      GST_DEBUG_CATEGORY_INIT (player_debug, "CGstPlayerPipeline", 0, "CGstPlayerPipeline");
      GST_CAT_DEBUG( player_debug, "Constructor" );
      if ( !mPipeline.isValid() )
      {
         THROW_FATAL( PIPELINE_ERROR_MSG );
      }
      if ( !mFileSrc.isValid() )
      {
         THROW_FATAL( FILESRC_ERROR_MSG );
      }
      mLoop.reset( g_main_loop_new( NULL, FALSE ), finalizeMainLoop );
      mPipeline.setBusCallback( boost::bind( &CGstPlayerPipeline::onBusCall, self(), _1, _2 ) );
      boost::thread mainLoopRunner = boost::thread( [this]() {
         GST_CAT_DEBUG( player_debug, "Main loop started" );
         g_main_loop_run( mLoop.get() );
         GST_CAT_DEBUG( player_debug, "Main loop stopped" );
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
      GST_CAT_DEBUG( player_debug, "startPlaying" );
      if ( !mPipeline.isInState( GST_STATE_PLAYING ) )
      {
         {
            boost::lock_guard<boost::mutex> lock( mConditionGuard );
            mIsEos = false;
            GST_CAT_DEBUG( player_debug, "mIsEos set to false" );
         }
         mFileSrc.setProperty( FILESRC_LOCATION_PARAM, filename );
         GST_CAT_DEBUG( player_debug, "Set file name %s", filename.c_str() );
         result = mPipeline.setState( GST_STATE_PLAYING, true );
         GST_CAT_DEBUG( player_debug, "Set state result: %d", result );
      }
      return result;
   }

   void stopPlaying( void )
   {
      GST_CAT_DEBUG( player_debug, "stopPlaying" );
      if ( mPipeline.getState() != NULL )
      {
         if ( !mPipeline.setState( GST_STATE_NULL ) )
         {
            GST_CAT_ERROR( player_debug, "Could not set pipeline state to NULL" );
            THROW_FATAL( PLAYER_ERROR_MSG );
         }
         {
            boost::lock_guard<boost::mutex> lock( mConditionGuard );
            mIsEos = true;
            GST_CAT_DEBUG( player_debug, "mIsEos set to true" );
         }
         GST_CAT_DEBUG( player_debug, "Notify all" );
         mCondition.notify_all();
      }
   }

   bool waitForCompletion( void )
   {
      boost::unique_lock<boost::mutex> lock( mConditionGuard );
      GST_CAT_DEBUG( player_debug, "waitForCompletion..." );
      bool result = mCondition.wait_for( lock, 
         boost::chrono::seconds( MAX_PLAYBACK_DURATION_SEC ), 
         boost::bind( &CGstPlayerPipeline::isEos, this ) );
      GST_CAT_DEBUG( player_debug, "waitForCompletion result: %d", result );
      return result;
   }

   bool isPlaying( void )
   {
      bool result = mPipeline.isInState( GST_STATE_PLAYING );
      GST_CAT_DEBUG( player_debug, "isPlaying: %", result );
      return result;
   }

   ~CGstPlayerPipeline( void )
   {
      GST_CAT_DEBUG( player_debug, "destructor" );
      if ( isPlaying() )
      {
         stopPlaying();
      }
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
         GST_CAT_DEBUG( player_debug, "EOS message received on BUS" );
         {
            boost::lock_guard<boost::mutex> lock( mConditionGuard );
            mIsEos = true;
            GST_CAT_DEBUG( player_debug, "mIsEos set to true" );
         }
         GST_CAT_DEBUG( player_debug, "Notify all" );
         mCondition.notify_all();
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

private:
   CGstPipeline mPipeline;
   CGstElement mFileSrc;
   CGstElement mSink;
   CGstPad mSinkPad;
   MainLoopPtr mLoop;
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
   if ( mNotifierThread.joinable() )
   {
      mNotifierThread.interrupt();
      mNotifierThread.join();
   }
}

bool CFilePlayer::startPlaying( const std::string& fileName )
{
   bool result = false;
   std::string file( fileName );
   
   result = mPlayerPipeline->startPlaying( fileName );
   if ( result )
   {
      mNotifierThread = boost::thread( [this, file]() {
         bool result = false;
         try
         {
            GST_CAT_DEBUG( player_debug, "mNotifierThread wait for complition..." );
            result = mPlayerPipeline->waitForCompletion();
            GST_CAT_DEBUG( player_debug, "mNotifierThread wait for complition result: %d", result );
         }
         catch (...)
         {

         }
         if ( !result )
         {
            THROW_FATAL( PLAYER_ERROR_MSG );
         }
         mStopPlaying( StopPlayingData( file ) );
      } );
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
   GST_CAT_DEBUG( player_debug, "playFile" );
   bool result = startPlaying( fileName );
   if ( result )
   {
      GST_CAT_DEBUG( player_debug, "main_thread wait for complition..." );
      result = mPlayerPipeline->waitForCompletion();
      GST_CAT_DEBUG( player_debug, "main_thread wait for complition result: %d", result );
      if ( !result )
      {
         THROW_FATAL( PLAYER_ERROR_MSG );
      }
      mPlayerPipeline->stopPlaying();
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