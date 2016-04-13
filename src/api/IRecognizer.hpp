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

#include "Signals.hpp"

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

   struct StartListeningData
   {
      RecognizerMode::eRecognizerMode mode;
      bool status;

      StartListeningData( RecognizerMode::eRecognizerMode _mode, bool _status )
         : mode( _mode )
         , status( _status )
      {

      }
   };

   struct RecognitionResultData
   {
      bool status;
      std::string text;

      RecognitionResultData( const std::string& _text, bool _status )
         : text( _text )
         , status( _status )
      {

      }

   };

   struct StopListeningData {};

   typedef signals::signal<void ( const StartListeningData& e )> StartListening;
   typedef signals::signal<void ( const RecognitionResultData& e )> RecognitionResult;
   typedef signals::signal<void ( const StopListeningData& e )> StopListening;

   class IRecognizer
   {
   public:
      virtual ~IRecognizer( void ) = 0;
      virtual std::string getLanguage( void ) const = 0;
      virtual void setLanguage( const std::string& language ) = 0;
      virtual api::RecognizerMode::eRecognizerMode getMode( void ) const = 0;
      virtual bool setMode( api::RecognizerMode::eRecognizerMode mode ) = 0;
      virtual bool listen( void ) = 0;
      virtual bool isListening( void ) const = 0;
      virtual void stopListening( void ) = 0;

      virtual signals::connection subscribe( const StartListening::slot_type& slot ) = 0;
      virtual signals::connection subscribe( const RecognitionResult::slot_type& slot ) = 0;
      virtual signals::connection subscribe( const StopListening::slot_type& slot ) = 0;
   };
}