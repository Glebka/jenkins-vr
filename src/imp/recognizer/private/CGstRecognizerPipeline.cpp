/*************************************************************************
 * jenkins-vr
 *************************************************************************
 * @file    CGstRecognizerPipeline.cpp
 * @date    26.06.16
 * @author  Hlieb Romanov
 * @brief   Pipeline with pocketsphinx decoder
 ************************************************************************/
#include <boost/thread.hpp>
#include <boost/format.hpp>

#include <glib.h>
#include <glib/gstdio.h>

#include "CGstRecognizerPipeline.hpp"
#include "CDecoder.hpp"
#include "imp/logger/CLogger.hpp"

GST_DEBUG_CATEGORY_STATIC( recognizer_debug );

static const guint64 MAX_WAIT_TIMEOUT = 5 * GST_SECOND;

static const char* PIPELINE_SPEC = "directsoundsrc name=asrc ! audioconvert ! audioresample ! pocketsphinx name=asr ! fakesink";
static const char* ASR_NAME = "asr";
static const char* AUDIO_SOURCE = "asrc";
static const char* ASR_HMM_PARAM = "hmm";
static const char* ASR_DICT_PARAM = "dict";
static const char* ASR_DECODER_PARAM = "decoder";

static const char* PIPELINE_ERROR_MSG = "Can't create the recognizer pipeline, GStreamer returns NULL";
static const char* POCKETSPHINX_ERROR_MSG = "Can't create the pocketsphinx element, GStreamer returns NULL";
static const char* RECOGNIZER_ERROR_MSG = "Recognizer may be in inconsistent state. Aborting.";
static const char* PARSE_ERROR_MSG = "GStreamer error (%1%): %2%";

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
   GST_CAT_ERROR( recognizer_debug, message.c_str() );
   throw std::runtime_error( message );
}

CGstRecognizerPipeline::CGstRecognizerPipeline( const std::string& hmmDir, const std::string& dictFile )
   : mListening( false )
   , mIsEosReceived( false )
   , mPipeline( PIPELINE_SPEC )
   , mPocketSphinx( mPipeline.getElementByName( ASR_NAME ) )
   , mLoop( NULL ) 
{
   GST_DEBUG_CATEGORY_INIT( recognizer_debug, "CGstRecognizerPipeline", 0, "CGstRecognizerPipeline" );
   GST_CAT_DEBUG( recognizer_debug, "Constructor" );
   if ( !mPipeline.isValid() )
   {
      THROW_FATAL( PIPELINE_ERROR_MSG );
   }
   if ( !mPocketSphinx.isValid() )
   {
      THROW_FATAL( POCKETSPHINX_ERROR_MSG );
   }
   GST_CAT_DEBUG( recognizer_debug, "Setting pocketsphinx HMM and dict files..." );
   mPocketSphinx.setProperty( ASR_HMM_PARAM, hmmDir );
   mPocketSphinx.setProperty( ASR_DICT_PARAM, dictFile );
   GST_CAT_DEBUG( recognizer_debug, "Setting bus callback..." );
   mPipeline.setBusCallback( boost::bind( &CGstRecognizerPipeline::onBusCall, self(), _1, _2 ) );
   GST_CAT_DEBUG( recognizer_debug, "Creating main loop..." );
   mLoop.reset( g_main_loop_new( NULL, FALSE ), finalizeMainLoop );
   boost::thread mainLoopRunner = boost::thread( [this]() {
      GST_CAT_DEBUG( recognizer_debug, "Main loop started" );
      g_main_loop_run( mLoop.get() );
      GST_CAT_DEBUG( recognizer_debug, "Main loop stopped" );
   } );
   mainLoopRunner.detach();
   GST_CAT_DEBUG( recognizer_debug, "Trying to initialize recognizer" );
   if ( !initialize() )
   {
      THROW_FATAL( RECOGNIZER_ERROR_MSG );
   }
   GST_CAT_DEBUG( recognizer_debug, "Recognizer is initialized" );

}

CGstRecognizerPipeline::~CGstRecognizerPipeline( void )
{
   GST_CAT_DEBUG( recognizer_debug, "Destructor" );
   if ( isInitialized() )
   {
      deinitialize();
   }
}

CGstRecognizerPipeline* CGstRecognizerPipeline::self( void )
{
   return this;
}

bool CGstRecognizerPipeline::isInitialized( void )
{
   return mPipeline.isInState( GST_STATE_READY ) || mListening;
}

bool CGstRecognizerPipeline::isListening( void )
{
   return mListening;
}

bool CGstRecognizerPipeline::startListening( void )
{
   GST_CAT_DEBUG( recognizer_debug, "Start listening" );
   bool result = false;
   if ( isInitialized() && !isListening() )
   {
      GST_CAT_DEBUG( recognizer_debug, "Setting state to GST_STATE_PLAYING ASYNC..." );
      result = mPipeline.setState( GST_STATE_PLAYING, true );
      GST_CAT_DEBUG( recognizer_debug, "Set state result: %d", result );
      mListening = result;
      boost::lock_guard<boost::mutex> lock( mEosGuard );
      mIsEosReceived = false;
   }
   return result;
}

void CGstRecognizerPipeline::stopListening( void )
{
   GST_CAT_DEBUG( recognizer_debug, "Stop listening" );
   if ( isListening() )
   {
      GST_CAT_DEBUG( recognizer_debug, "Send EOS event" );
      mPipeline.getElementByName( AUDIO_SOURCE ).sendEvent( gst_event_new_eos() );
      GST_CAT_DEBUG( recognizer_debug, "EOS event is sent" );
      {
         GST_CAT_DEBUG( recognizer_debug, "Start waiting for EOS on the BUS" );
         boost::unique_lock<boost::mutex> lock( mEosGuard );
         if ( !mEosCondition.wait_for( lock, boost::chrono::nanoseconds( MAX_WAIT_TIMEOUT ), 
            [this]( void ){ return mIsEosReceived; } ) )
         {
            THROW_FATAL( RECOGNIZER_ERROR_MSG );
         }
         GST_CAT_DEBUG( recognizer_debug, "EOS wait is finished" );
      }
      GST_CAT_DEBUG( recognizer_debug, "Setting state to GST_STATE_READY ASYNC..." );
      mListening = !mPipeline.setState( GST_STATE_READY );
      GST_CAT_DEBUG( recognizer_debug, "Set state result: %d", !mListening );
   }
}

DecoderPtr CGstRecognizerPipeline::getDecoder( void )
{
   return mDecoder;
}

bool CGstRecognizerPipeline::initialize( void )
{
   GST_CAT_DEBUG( recognizer_debug, "Initialize" );
   if ( !isInitialized() )
   {
      GST_CAT_DEBUG( recognizer_debug, "Setting state to GST_STATE_READY ASYNC..." );
      bool result = mPipeline.setState( GST_STATE_READY, true );
      GST_CAT_DEBUG( recognizer_debug, "Set state result: %d", result );
   }
   bool result = isInitialized();
   GST_CAT_DEBUG( recognizer_debug, "isInitialized: %d", result );
   if ( result )
   {
      GST_CAT_DEBUG( recognizer_debug, "Getting ps_decoder object..." );
      ps_decoder_t* decoder = mPocketSphinx.getProperty<ps_decoder_t*>( ASR_DECODER_PARAM );
      mDecoder.reset( new CDecoder( decoder ) );
      GST_CAT_DEBUG( recognizer_debug, "ps_decoder object is wrapped" );
   }
   return result;
}

void CGstRecognizerPipeline::deinitialize( void )
{
   GST_CAT_DEBUG( recognizer_debug, "deinitialize" );
   if ( isInitialized() )
   {
      GST_CAT_DEBUG( recognizer_debug, "Setting state to GST_STATE_NULL ASYNC..." );
      bool result = mPipeline.setState( GST_STATE_NULL );
      GST_CAT_DEBUG( recognizer_debug, "Set state result: %d", result );
   }
   if ( isInitialized() )
   {
      THROW_FATAL( RECOGNIZER_ERROR_MSG );
   }
   else
   {
      GST_CAT_DEBUG( recognizer_debug, "Deinitialize is completed" );
   }
}

void CGstRecognizerPipeline::onBusCall( GstBus* bus, GstMessage* msg )
{
   switch ( GST_MESSAGE_TYPE( msg ) )
   {
   case GST_MESSAGE_EOS:
      GST_CAT_DEBUG( recognizer_debug, "Got EOS on the BUS" );
      {
         boost::lock_guard<boost::mutex> lock( mEosGuard );
         mIsEosReceived = true;
      }
      GST_CAT_DEBUG( recognizer_debug, "Notify all waiters" );
      mEosCondition.notify_all();
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
   const GstStructure *st = gst_message_get_structure(msg);
   if (st && strcmp(gst_structure_get_name(st), "pocketsphinx") == 0) {
	   if (g_value_get_boolean(gst_structure_get_value(st, "final")))
    	   g_print("Got result %s, confidence: %ld\n", 
            g_value_get_string(gst_structure_get_value(st, "hypothesis")),
            g_value_get_long(gst_structure_get_value(st, "confidence"))
         );
    }
}