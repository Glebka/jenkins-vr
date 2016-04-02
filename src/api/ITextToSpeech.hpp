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

namespace api
{
   class ITextToSpeech;

   typedef boost::shared_ptr<ITextToSpeech> TextToSpeechPtr;

   class ITextToSpeech
   {
      virtual ~ITextToSpeech( void ) = 0;

      virtual std::vector<std::string> getAvailableLanguages( void ) const = 0;
      virtual std::string getCurrentLanguage( void ) const = 0;
      virtual bool setLanguage( const std::string& language ) = 0;
      virtual bool sayAsync( const std::string& text ) = 0;
      virtual bool saySync( const std::string& text ) = 0;
      virtual bool isSpeaking( void ) const = 0;
      virtual void stopSpeaking( void ) = 0;
   };
}