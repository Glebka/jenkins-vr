#include <iostream>
#include <gst/gst.h>
#include <vector>

#include "imp/logger/CLogger.hpp"

#ifdef _WIN32

#include "imp/tts/CWinTTS.hpp"

#endif

void handler1( api::tts::StartSpeakingData data )
{
   CLogger::info() << "StartSpeaking event: " << data.status << "; '" << data.text << "'";
}

void handler2( api::tts::StopSpeakingData data )
{
   CLogger::info() << "StopSpeaking event";
}

int main()
{
	gst_init( NULL, NULL );

   CLogger::fatal() << "Hello fatal! " << "This is on the same line.";
   CLogger::error() << "Hello error! " << "This is on the same line.";
   CLogger::warning() << "Hello warning! " << "This is on the same line.";
   CLogger::info() << "Hello info! " << "This is on the same line.";
   CLogger::debug() << "Hello debug! " << "This is on the same line.";

#ifdef _WIN32
   api::tts::TextToSpeechPtr tts = CWinTTS::create();

   std::cout << tts->getCurrentLanguage() << std::endl;
   tts->setLanguage( "en-GB" );
   std::cout << tts->getCurrentLanguage() << std::endl;
   tts->onStartSpeaking( handler1 );
   tts->onStopSpeaking( handler2 );
   tts->saySync( "This is a test string for computer voice" );
   system("pause");
   tts->sayAsync( "This is a test string for computer voice" );
   system("pause");
#endif
	return 0;
}