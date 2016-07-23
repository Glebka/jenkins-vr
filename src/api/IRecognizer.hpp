/*************************************************************************
 * jenkins-vr
 *************************************************************************
 * @file    IRecognizer.hpp
 * @date    02.04.16
 * @author  Hlieb Romanov
 * @brief   Speech recognizer interface declaration
 ************************************************************************/
#pragma once

#include <string>
#include <boost/shared_ptr.hpp>
#include <vector>

#include "Signals.hpp"

/**
 * Syntax - Message
 */
namespace api
{
   namespace asr
   {
      class IRecognizer;   ///< Forward declaration of speech recognizer interface
      typedef boost::shared_ptr<IRecognizer> RecognizerPtr;
      typedef std::pair<std::string, std::string> GraphemePhoneme;   ///< Word and it's pronunciation
      typedef std::vector<GraphemePhoneme> GraphemePhonemeList;      ///< List of words and their pronunciations

      namespace RecognizerMode
      {
         enum eRecognizerMode
         {
            NONE,
            KEY_WORD_SEARCH,  ///< Recognize magic phrazes like "Ok, Google!"
            GRAMMAR_SEARCH    ///< Recognize commands specified by grammar
         };
      }

      /**
       * StartListening signal data.
       * Instance of this structure will be sent to the handler.
       * @sa StartListeningSignal_t
       */
      struct StartListeningData
      {
         RecognizerMode::eRecognizerMode mode;
         bool status;   ///< Status of the operation

         StartListeningData( RecognizerMode::eRecognizerMode _mode, bool _status )
            : mode( _mode )
            , status( _status )
         {

         }
      };

      /**
       * This stucture is passed to the RecognitionResult signal handler
       * and provides recognition results as text.
       * @sa RecognitionResultSignal_t
       */
      struct RecognitionResultData
      {
         bool status;      ///< Status of the operation
         std::string text; ///< Recognized text

         RecognitionResultData( const std::string& _text, bool _status )
            : text( _text )
            , status( _status )
         {

         }

      };

      /**
       * StopListening signal data (empty)
       */
      struct StopListeningData {};

      typedef signals::signal<void ( StartListeningData e )> StartListeningSignal_t;        ///< StartListening signal type
      typedef signals::signal<void ( RecognitionResultData e )> RecognitionResultSignal_t;  ///< RecognitionResult signal type
      typedef signals::signal<void ( StopListeningData e )> StopListeningSignal_t;          ///< StopListening signal type

      /**
       * Speech recognizer interface class
       */
      class IRecognizer
      {
      public:
         virtual ~IRecognizer( void ) = 0 {}

         /**
          * Gets current recognition language
          */
         virtual std::string getLanguage( void ) const = 0;

         /**
          * Sets current recognition language
          */
         virtual void setLanguage( const std::string& language ) = 0;

         /**
          * Gets current recognition mode
          */
         virtual api::asr::RecognizerMode::eRecognizerMode getMode( void ) const = 0;

         /**
          * Sets current recognition mode
          */
         virtual bool setMode( api::asr::RecognizerMode::eRecognizerMode mode ) = 0;

         /**
          * Start listening to the audio stream from the microphone.
          * Emits StartListening signal.
          * Recognizer emits RecognitionResult signal if it recognized some speech.
          * @sa api::asr::StartListeningSignal_t
          * @sa api::asr::RecognitionResultSignal_t
          * @sa onStartListening()
          * @sa onRecognitionResult()
          */
         virtual bool listen( void ) = 0;

         /**
          * Checks whether this recognizer is listening to the audio or not.
          */
         virtual bool isListening( void ) const = 0;

         /**
          * Stops listening to the audio and emits StopListening signal.
          * @sa api::asr::StopListeningSignal_t
          * @sa onStopListening()
          */
         virtual void stopListening( void ) = 0;

         /**
          * Extends existing pronunciation dictionary and language model with new words.
          * Previous phonetization will be discarded.
          * @param group - the group of entities that should be updated, for example, 'projects'
          * @param g2pList - list of pairs 'grapheme' - 'phoneme' that should be added to the dictionary and grammar
          */
         virtual bool phonetize( const std::string& group, const GraphemePhonemeList& g2pList ) = 0;

         /**
          * Register handler for StartListening signal.
          * @sa api::asr::StartListeningSignal_t
          * @sa api::asr::StartListeningData
          */
         virtual signals::connection onStartListening( const StartListeningSignal_t::slot_type& slot ) = 0;

         /**
          * Register handler for RecognitionResult signal.
          * @sa api::asr::RecognitionResultSignal_t
          * @sa api::asr::RecognitionResultData
          */
         virtual signals::connection onRecognitionResult( const RecognitionResultSignal_t::slot_type& slot ) = 0;

         /**
          * Register handler for StopListening signal.
          * @sa api::asr::StopListeningSignal_t
          * @sa api::asr::StopListeningData
          */
         virtual signals::connection onStopListening( const StopListeningSignal_t::slot_type& slot ) = 0;
      };
   }
}