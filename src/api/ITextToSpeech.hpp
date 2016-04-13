/*************************************************************************
 * jenkins-vr
 *************************************************************************
 * @file    ITextToSpeech.hpp
 * @date    02.04.16
 * @author  Hlieb Romanov
 * @brief
 ************************************************************************/
#pragma once

#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "Signals.hpp"

namespace api
{
   class ITextToSpeech;

   typedef boost::shared_ptr<ITextToSpeech> TextToSpeechPtr;

   struct StartSpeakingData
   {
      std::string text;
      bool status;

      StartSpeakingData( const std::string& _text, bool _status )
         : text( _text )
         , status( _status )
      {

      }
   };

   struct StopSpeakingData {};

   typedef signals::signal<void ( const StartSpeakingData& e )> StartSpeaking;
   typedef signals::signal<void ( const StopSpeakingData& e )> StopSpeaking;

   class ITextToSpeech
   {
   public:
      virtual ~ITextToSpeech( void ) = 0;

      virtual std::vector<std::string> getAvailableLanguages( void ) const = 0;
      virtual std::string getCurrentLanguage( void ) const = 0;
      virtual bool setLanguage( const std::string& language ) = 0;
      virtual bool sayAsync( const std::string& text ) = 0;
      virtual bool saySync( const std::string& text ) = 0;
      virtual bool isSpeaking( void ) const = 0;
      virtual void stopSpeaking( void ) = 0;

      virtual signals::connection subscribe( const StartSpeaking::slot_type& slot ) = 0;
      virtual signals::connection subscribe( const StopSpeaking::slot_type& slot ) = 0;

   };
}