/*************************************************************************
 * jenkins-vr
 *************************************************************************
 * @file    CDecoder.cpp
 * @date    01.07.16
 * @author  Hlieb Romanov
 * @brief   Pocketsphinx decoder wrapper
 ************************************************************************/
#include <boost/assign.hpp>
#include <map>
#include "CDecoder.hpp"

typedef std::map<int, const char*> SearchModeToNameMap;

static const char* KW_SEARCH = "_kws";
static const char* GRAMMAR_SEARCH = "grammar";
static const char* NULL_SEARCH = "null";

static SearchModeToNameMap ModeToNameMap = boost::assign::map_list_of( api::asr::RecognizerMode::KEY_WORD_SEARCH, KW_SEARCH )
   ( api::asr::RecognizerMode::GRAMMAR_SEARCH, GRAMMAR_SEARCH );


CDecoder::CDecoder( ps_decoder_t* decoder )
{
   assert( decoder != NULL );
   mDecoder = decoder;
}

CDecoder::~CDecoder( void )
{
   ps_unset_search( mDecoder, KW_SEARCH );
   ps_unset_search( mDecoder, GRAMMAR_SEARCH );
   ps_free( mDecoder );
}

bool CDecoder::setKeyFile( const std::string& keyFile )
{
   return ( ps_set_kws( mDecoder, KW_SEARCH, keyFile.c_str() ) == 0 );
}

bool CDecoder::setKeyPhrase( const std::string& keyPhrase )
{
   return ( ps_set_keyphrase( mDecoder, KW_SEARCH, keyPhrase.c_str() ) == 0 );
}

bool CDecoder::addWordToDict( const api::asr::GraphemePhoneme& wordAndTranscript, bool updateDict )
{
   return ( ps_add_word( mDecoder, wordAndTranscript.first.c_str(), wordAndTranscript.second.c_str(), updateDict ) == 0 );
}

bool CDecoder::setGrammar( const std::string& grammarCode )
{
   return ( ps_set_jsgf_string( mDecoder, GRAMMAR_SEARCH, grammarCode.c_str() ) == 0 );
}

bool CDecoder::setGrammarFile( const std::string& grammarFile )
{
   return ( ps_set_jsgf_file( mDecoder, GRAMMAR_SEARCH, grammarFile.c_str() ) == 0 );
}

bool CDecoder::activateMode( api::asr::RecognizerMode::eRecognizerMode mode )
{
   return ( ps_set_search( mDecoder, ModeToNameMap[ mode ] ) == 0 );
}

bool CDecoder::endUtterance( void )
{
   return ( ps_end_utt( mDecoder ) == 0 );
}

