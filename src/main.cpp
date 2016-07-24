#include <iostream>
#include <gst/gst.h>
#include <vector>

#include "api/ITextToSpeech.hpp"
#include "imp/logger/CLogger.hpp"
#include "imp/player/CFilePlayer.hpp"
#include "imp/recognizer/CSphinxRecognizer.hpp"
#include <pocketsphinx.h>
#include <glib.h>
#include <glib/gstdio.h>

#ifdef _WIN32

#include "imp/tts/CWinTTS.hpp"

#endif

GST_DEBUG_CATEGORY_STATIC (app_debug);

void handler1( api::tts::StartSpeakingData data )
{
   CLogger::info() << "StartSpeaking event: " << data.status << "; '" << data.text << "'";
}

void handler2( api::tts::StopSpeakingData data )
{
   CLogger::info() << "StopSpeaking event";
}

void handler3( api::player::StartPlayingData data )
{
   CLogger::info() << "StartPlaying event: " << data.status << "; '" << data.file << "'";
}

void handler4( api::player::StopPlayingData data )
{
   CLogger::info() << "StopPlaying event: " << data.file;
}

int main()
{
	gst_init( NULL, NULL );

   GST_DEBUG_CATEGORY_INIT (app_debug, "JENKINS-VR", 0, "JENKINS-VR");
   FILE* mLogFile = g_fopen( "logfile.txt", "w" );

   if ( mLogFile )
   {
      bool result = true;
      gst_debug_remove_log_function( NULL );
      gst_debug_add_log_function( gst_debug_log_default, mLogFile, NULL );
      gst_debug_set_threshold_from_string( "GST_TRACER:7,GST_BUFFER*:7,GST_EVENT:7,GST_MESSAGE:7,JENKINS-VR:7,pocketsphinx:7,CGstPlayerPipeline:7,CGstRecognizerPipeline:7", true );
   }

   CLogger::setConsoleLogLevel( LogLevel::LEVEL_DEBUG );
   CLogger::fatal() << "Hello fatal! " << "This is on the same line.";
   CLogger::error() << "Hello error! " << "This is on the same line.";
   CLogger::warning() << "Hello warning! " << "This is on the same line.";
   CLogger::info() << "Hello info! " << "This is on the same line.";
   CLogger::debug() << "Hello debug! " << "This is on the same line.";
	return 0;
}