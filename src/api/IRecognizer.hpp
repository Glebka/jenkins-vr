/*************************************************************************
 * jenkins-vr
 *************************************************************************
 * @file    IRecognizer.hpp
 * @date    02.04.16
 * @author  Hlieb Romanov
 * @brief
 ************************************************************************/
#pragma once

#include <string>
#include <boost/shared_ptr.hpp>

namespace api
{
   class IRecognizer;
   typedef boost::shared_ptr<IRecognizer> RecognizerPtr;

   namespace RecognizerMode
   {
      enum eRecognizerMode
      {
         NONE,
         KEY_WORD_SEARCH,
         GRAMMAR_SEARCH
      };
   }

   class IRecognizer
   {
      virtual ~IRecognizer( void ) = 0;
      virtual std::string getLanguage( void ) const = 0;
      virtual void setLanguage( const std::string& language ) = 0;
      virtual api::RecognizerMode::eRecognizerMode getMode( void ) const = 0;
      virtual bool setMode( api::RecognizerMode::eRecognizerMode mode ) = 0;
      virtual bool listen( void ) = 0;
      virtual bool isListening( void ) const = 0;
      virtual void stopListening( void ) = 0;
   };
}