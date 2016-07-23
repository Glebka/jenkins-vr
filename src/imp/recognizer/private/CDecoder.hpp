/*************************************************************************
 * jenkins-vr
 *************************************************************************
 * @file    CDecoder.hpp
 * @date    28.06.16
 * @author  Hlieb Romanov
 * @brief   Pocketsphinx decoder wrapper
 ************************************************************************/
#pragma once

#include <boost/noncopyable.hpp>
#include <string>

#include <api/IRecognizer.hpp>
#include <pocketsphinx.h>

class CDecoder: boost::noncopyable
{
public:
   CDecoder( ps_decoder_t* decoder );
   ~CDecoder( void );

   bool setKeyPhrase( const std::string& keyPhrase );
   bool setKeyFile( const std::string& keyFile );
   bool addWordToDict( const api::asr::GraphemePhoneme& wordAndTranscript, bool updateDict = false );
   bool setGrammarFile( const std::string& grammarFile );
   bool setGrammar( const std::string& grammarCode );
   bool activateMode( api::asr::RecognizerMode::eRecognizerMode mode );
   bool endUtterance( void );

private:
   ps_decoder_t* mDecoder;
};