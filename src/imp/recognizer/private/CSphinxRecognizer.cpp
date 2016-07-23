/*************************************************************************
 * jenkins-vr
 *************************************************************************
 * @file    CSphinxRecognizer.cpp
 * @date    14.06.16
 * @author  Hlieb Romanov
 * @brief   Speech recognizer class implementation
 ************************************************************************/
#include <gst/gst.h>
#include <glib.h>
#include <boost/format.hpp>
#include <boost/thread.hpp>

#include "imp/recognizer/CSphinxRecognizer.hpp"
#include "imp/recognizer/private/CGstRecognizerPipeline.hpp"
#include "imp/recognizer/private/CDecoder.hpp"
#include "imp/logger/CLogger.hpp"

using namespace api::asr;

/**
 * TODO: create IConfigurable interface
 */
static const char* DEFAULT_LANG_DIR = "lang";
static const char* DEFAULT_LANGUAGE = "ru-RU";
static const char* KEY_FILE_EXTENSION = ".key";
static const char* DICT_FILE_EXTENSION = ".dic";
static const char* GRAMMAR_FILE_EXTENSION = ".jsgf";
static const char* RECOGNIZER_ERROR_MSG = "Recognizer may be in inconsistent state. Aborting.";
static const char* RECOGNIZER_KWS_ERROR_MSG = "Unable to configure key word recognition. Aborting.";
static const char* RECOGNIZER_VR_ERROR_MSG = "Unable to configure voice recognition. Aborting.";

/**
 * TODO: make common hpp and cpp files and move utility functions to it.
 */
static void THROW_FATAL( const std::string&  message )
{
   CLogger::fatal() << message;
   throw std::runtime_error( message );
}

static void RUN_CHECKED( bool operationResult, const std::string& errorMessage )
{
   if ( !operationResult )
   {
      THROW_FATAL( errorMessage );
   }
}

CSphinxRecognizer::CSphinxRecognizer( void )
   : mLanguage( DEFAULT_LANGUAGE )
   , mMode( RecognizerMode::KEY_WORD_SEARCH )
{
   reinit();
}

void CSphinxRecognizer::reinit( void )
{
   std::string langDir( DEFAULT_LANG_DIR );
   langDir += "\\" + mLanguage;
   std::string dictDir = langDir + "\\" + mLanguage + std::string( DICT_FILE_EXTENSION );
   std::string keyFile = langDir + "\\" + mLanguage + std::string( KEY_FILE_EXTENSION );
   std::string grammarFile = langDir + "\\" + mLanguage + std::string( GRAMMAR_FILE_EXTENSION );
   mRecognizerPipeline.reset( new CGstRecognizerPipeline( langDir, dictDir ) );
   DecoderPtr decoder = mRecognizerPipeline->getDecoder();
   RUN_CHECKED( decoder->setKeyFile( keyFile ), RECOGNIZER_KWS_ERROR_MSG );
   RUN_CHECKED( decoder->setGrammarFile( grammarFile ), RECOGNIZER_VR_ERROR_MSG );
   RUN_CHECKED( decoder->activateMode( mMode ), RECOGNIZER_VR_ERROR_MSG );
}

api::asr::RecognizerPtr CSphinxRecognizer::create( void )
{
   return RecognizerPtr( new CSphinxRecognizer() );
}


CSphinxRecognizer::~CSphinxRecognizer( void )
{
}


std::string CSphinxRecognizer::getLanguage( void ) const
{
   return mLanguage;
}


void CSphinxRecognizer::setLanguage( const std::string& language )
{
   mLanguage = language;
   reinit();
}


api::asr::RecognizerMode::eRecognizerMode CSphinxRecognizer::getMode( void ) const
{
   return mMode;
}


bool CSphinxRecognizer::setMode( api::asr::RecognizerMode::eRecognizerMode mode )
{
   bool result = false;
   if ( !mRecognizerPipeline->isListening() )
   {
      DecoderPtr decoder = mRecognizerPipeline->getDecoder();
      if ( decoder )
      {
         result = decoder->activateMode( mode );
      }
   }
   return result;
}


bool CSphinxRecognizer::listen( void )
{
   bool result = false;
   if ( !mRecognizerPipeline->isListening() && mMode != RecognizerMode::NONE )
   {
      result = mRecognizerPipeline->startListening();
      mStartListening( StartListeningData( mMode, result ) );
   }
   return result;
}


bool CSphinxRecognizer::isListening( void ) const
{
   return mRecognizerPipeline->isListening();
}


void CSphinxRecognizer::stopListening( void )
{
   if ( mRecognizerPipeline->isListening() )
   {
      if ( !mRecognizerPipeline->stopListening() )
      {
         THROW_FATAL( RECOGNIZER_ERROR_MSG );
      }
      else
      {
         mStopListening( api::asr::StopListeningData() );
      }
   }
}

bool CSphinxRecognizer::phonetize( const std::string& group, const GraphemePhonemeList& g2pList )
{
   return false;
}

signals::connection CSphinxRecognizer::onStartListening( const api::asr::StartListeningSignal_t::slot_type& slot )
{
   return mStartListening.connect( slot );
}


signals::connection CSphinxRecognizer::onRecognitionResult( const api::asr::RecognitionResultSignal_t::slot_type& slot )
{
   return mRecognitionResult.connect( slot );
}


signals::connection CSphinxRecognizer::onStopListening( const api::asr::StopListeningSignal_t::slot_type& slot )
{
   return mStopListening.connect( slot );
}