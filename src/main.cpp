#include <iostream>
#include <gst/gst.h>
#include <vector>

#include "imp/logger/CLogger.hpp"

#ifdef _WIN32

#include "imp/tts/CWinTTS.hpp"

#endif


int main()
{
	gst_init( NULL, NULL );

   CLogger::fatal() << "Hello fatal! " << "This is on the same line.";
   CLogger::error() << "Hello error! " << "This is on the same line.";
   CLogger::warning() << "Hello warning! " << "This is on the same line.";
   CLogger::info() << "Hello info! " << "This is on the same line.";
   CLogger::debug() << "Hello debug! " << "This is on the same line.";

#ifdef _WIN32
   api::TextToSpeechPtr tts = CWinTTS::create();

   std::cout << tts->getCurrentLanguage() << std::endl;
   tts->setLanguage( "en-US" );
   std::cout << tts->getCurrentLanguage() << std::endl;
   tts->saySync( "Hello! I'm glad to see you!" );
   tts->setLanguage( "ru-RU" );
   tts->saySync( "Hello! I'm glad to see you!" );
   system("pause");
#endif
	return 0;
}