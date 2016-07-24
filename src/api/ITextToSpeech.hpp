/*************************************************************************
 * jenkins-vr
 *************************************************************************
 * @file    ITextToSpeech.hpp
 * @date    02.04.16
 * @author  Hlieb Romanov
 * @brief   TTS interface declaration
 ************************************************************************/
#pragma once

#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>
#include "Signals.hpp"

namespace api
{
   namespace tts
   {
      class ITextToSpeech; ///< Forward declaration of TTS interface

      typedef boost::shared_ptr<ITextToSpeech> TextToSpeechPtr;

      /**
       * StartSpeaking event data.
       * The instance of this struct is passed to event handler.
       * @sa api::tts::ITextToSpeech::saySync()
       * @sa api::tts::ITextToSpeech::sayAsync()
       * @sa api::tts::ITextToSpeech::onStartSpeaking()
       * @sa api::tts::StartSpeakingSignal_t
       */
      struct StartSpeakingData
      {
         std::string text;
         bool status;

         /**
          * Class constructor
          * @param _text - text to say
          * @param _status - status of api::tts::ITextToSpeech::sayAsync() or api::tts::ITextToSpeech::saySync() operations
          */
         StartSpeakingData( const std::string& _text, bool _status )
            : text( _text )
            , status( _status )
         {

         }
      };

      /**
       * StopSpeaking event data (empty).
       * @sa api::tts::ITextToSpeech::saySync()
       * @sa api::tts::ITextToSpeech::stopSpeaking()
       * @sa api::tts::ITextToSpeech::onStopSpeaking()
       * @sa api::tts::StopSpeakingSignal_t
       */
      struct StopSpeakingData {};

      typedef signals::signal<void ( StartSpeakingData e )> StartSpeakingSignal_t; ///< StartSpeaking signal type
      typedef signals::signal<void ( StopSpeakingData e )> StopSpeakingSignal_t;   ///< StopSpeaking signal type

      /**
       * TTS interface class
       */
      class ITextToSpeech
      {
      public:
         virtual ~ITextToSpeech( void ) = 0;

         /**
          * Returns available TTS languages like en-US, en-GB, ru-RU, etc.
          * @return vector of available TTS languages
          */
         virtual std::vector<std::string> getAvailableLanguages( void ) const = 0;

         /**
          * Gets the current language of the voice
          */
         virtual std::string getCurrentLanguage( void ) const = 0;

         /**
          * Sets the voice that supports specified language
          */
         virtual bool setLanguage( const std::string& language ) = 0;

         /**
          * Starts saying specified text and returns immediately.
          * Emits StartSpeaking signal. StopSpeaking signal will be emitted after the end of speach.
          * @sa api::tts::StartSpeakingSignal_t
          * @sa api::tts::StartSpeakingData
          * @sa api::tts::StopSpeakingSignal_t
          * @sa api::tts::StopSpeakingData
          * @sa onStartSpeaking()
          * @sa onStopSpeaking()
          */
         virtual bool sayAsync( const std::string& text ) = 0;

         /**
          * Starts saying specified text and waits for completion.
          * Emits StartSpeaking signal at start and StopSpeaking signal - at the end.
          * @sa api::tts::StartSpeakingSignal_t
          * @sa api::tts::StartSpeakingData
          * @sa api::tts::StopSpeakingSignal_t
          * @sa api::tts::StopSpeakingData
          * @sa onStartSpeaking()
          * @sa onStopSpeaking()
          */
         virtual bool saySync( const std::string& text ) = 0;

         /**
          * Checks the status of the TTS engine
          */
         virtual bool isSpeaking( void ) const = 0;

         /**
          * Interrupts current speach.
          * As a result the StopSpeaking signal will be emitted.
          * @sa sayAsync()
          * @sa saySync()
          * @sa api::tts::StopSpeakingSignal_t
          */
         virtual void stopSpeaking( void ) = 0;

         /**
          * Registers handler for StartSpeaking signal
          * @sa api::tts::StartSpeakingSignal_t
          * @return signal-slot connection
          */
         virtual signals::connection onStartSpeaking( const StartSpeakingSignal_t::slot_type& slot ) = 0;

         /**
          * Registers handler for StopSpeaking signal
          * @sa api::tts::StartSpeakingSignal_t
          * @return signal-slot connection
          */
         virtual signals::connection onStopSpeaking( const StopSpeakingSignal_t::slot_type& slot ) = 0;
      };
   }
}