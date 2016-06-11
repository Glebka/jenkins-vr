/*************************************************************************
 * jenkins-vr
 *************************************************************************
 * @file    CWinTTS.cpp
 * @date    13.04.16
 * @author  Hlieb Romanov
 * @brief   Windows TTS class implementation
 ************************************************************************/
#ifdef _WIN32

#include <sstream>
#include <set>
#include <atlstr.h>
#include <boost/format.hpp>
#include <boost/thread.hpp>

#include "imp/tts/CWinTTS.hpp"
#include "imp/logger/CLogger.hpp"

using namespace api::tts;
using namespace details;

static const int MAX_WAIT_TIME_MS = 2 * 60 * 100;

static void WINAPI_RUN_CHECKED( HRESULT winApiCallResult, const std::string& errorMessage )
{
   if ( FAILED( winApiCallResult ) )
   {
      std::string message( "Can't initialize COM instance." );
      CLogger::fatal() << message;
      throw std::exception( message.c_str() );
   }
}

namespace details
{
   class ComContext
   {
   public:
      ComContext( void )
      {
         WINAPI_RUN_CHECKED( ::CoInitialize( NULL ), "Can't initialize COM instance." );
      }
      ~ComContext( void )
      {
         ::CoUninitialize();
      }
   };

   class SpVoiceToken : boost::noncopyable
   {
      SpVoiceToken( ISpObjectToken* voiceToken )
         : mVoiceToken( voiceToken )
      {
      }

   public:
      static SpVoiceTokenPtr create( ISpObjectToken* voiceToken )
      {
         return SpVoiceTokenPtr( new SpVoiceToken( voiceToken ) );
      }

   public:
      std::string getName( void )
      {
         assert( mVoiceToken != NULL );

         LPWSTR name = NULL;
         CComPtr<ISpDataKey> pAttrs;

         WINAPI_RUN_CHECKED( mVoiceToken->OpenKey( L"Attributes", &pAttrs ), "Can't get Attributes key from voice description." );
         WINAPI_RUN_CHECKED( pAttrs->GetStringValue( L"Name", &name ), "Can't get language code for voice" );
         return std::string( CW2A( name ) );
      }

      std::string getLanguage( void )
      {
         assert( mVoiceToken != NULL );

         const int MAX_LOCALE_LEN = 19;
         const int MAX_LOCALE_PART_LEN = 9;

         LPWSTR langCode = NULL;
         CComPtr<ISpDataKey> pAttrs;

         WINAPI_RUN_CHECKED( mVoiceToken->OpenKey( L"Attributes", &pAttrs ), "Can't get Attributes key from voice description." );
         WINAPI_RUN_CHECKED( pAttrs->GetStringValue( L"Language", &langCode ), "Can't get language code for voice" );

         std::wstringstream wss;
         wss << langCode;
         DWORD numericCode = 0;
         wss >> std::hex >> numericCode;

         char buf[MAX_LOCALE_LEN];
         int ccBuf = GetLocaleInfo( numericCode, LOCALE_SISO639LANGNAME, buf, MAX_LOCALE_PART_LEN );
         buf[ccBuf-1] = '-';
         ccBuf += GetLocaleInfo( numericCode, LOCALE_SISO3166CTRYNAME, buf+ccBuf, MAX_LOCALE_PART_LEN );

         return buf;
      }

      ISpObjectToken* raw( void )
      {
         return mVoiceToken;
      }

      ~SpVoiceToken( void )
      {
         if ( mVoiceToken )
         {
            mVoiceToken->Release();
         }
      }

   private:
      ISpObjectToken* mVoiceToken;
   };
}

CWinTTS::CWinTTS( void )
   : mComContext( new ComContext() )
   , mVoice( NULL )
{
   enumerateVoices();
   WINAPI_RUN_CHECKED( CoCreateInstance( CLSID_SpVoice, NULL, CLSCTX_ALL, IID_ISpVoice, (void **)&mVoice ), "Can't instantiate default voice." );
}

TextToSpeechPtr CWinTTS::create( void )
{
   return TextToSpeechPtr( new CWinTTS() );
}

CWinTTS::~CWinTTS( void )
{
   if ( mVoice )
   {
      mVoice->Release();
   }
}

std::vector<std::string> CWinTTS::getAvailableLanguages( void ) const
{
   return std::vector<std::string>( mLanguages.begin(), mLanguages.end() );
}

std::string CWinTTS::getCurrentLanguage( void ) const
{
   assert( mVoice != NULL );
   ISpObjectToken* cpVoiceToken = NULL;

   WINAPI_RUN_CHECKED( mVoice->GetVoice( &cpVoiceToken ), "Unable to get voice description for current voice." );
   SpVoiceTokenPtr voice = SpVoiceToken::create( cpVoiceToken );
   return voice->getLanguage();
}

bool CWinTTS::setLanguage( const std::string& language )
{
   bool result = false;
   VoicesIter it = mVoices.find( language );
   if ( it != mVoices.end() )
   {
      SpVoiceTokenPtr voice = it->second;
      WINAPI_RUN_CHECKED( mVoice->SetVoice( voice->raw() ), str(boost::format( "Can't set voice for %1%" ) % language) );
      result = true;
   }
   return result;
}

bool CWinTTS::sayAsync( const std::string& text )
{
   CA2W unicodeStr( text.c_str() );

   HRESULT hr = mVoice->Speak( unicodeStr, SPF_PURGEBEFORESPEAK | SPF_ASYNC, NULL );
   HANDLE waitHandle = mVoice->SpeakCompleteEvent();
   boost::thread notifier = boost::thread([this,waitHandle]( void ) {
      if ( WaitForSingleObject( waitHandle, MAX_WAIT_TIME_MS ) != WAIT_OBJECT_0 )
      {
         std::string message( "Can't initialize COM instance." );
         CLogger::fatal() << message;
         throw std::exception( message.c_str() );
      }
      mStopSpeakingSignal( StopSpeakingData() );
   });
   mStartSpeakingSignal( StartSpeakingData( text, true ) );
   return !FAILED( hr );
}

bool CWinTTS::saySync( const std::string& text )
{
   bool result = false;
   if ( sayAsync( text ) )
   {
      HRESULT hr = mVoice->WaitUntilDone( MAX_WAIT_TIME_MS + 1000 );
      result = !FAILED( hr );
   }
   return result;
}

bool CWinTTS::isSpeaking( void ) const
{
   SPVOICESTATUS status;
   WINAPI_RUN_CHECKED( mVoice->GetStatus( &status, NULL ), "Can't retrieve current voice status." );
   return status.dwRunningState != 0;
}

void CWinTTS::stopSpeaking( void )
{
   if ( isSpeaking() )
   {
      ULONG skipped = 0;
      WINAPI_RUN_CHECKED( mVoice->Skip( L"SENTENCE", INT_MAX, &skipped ), "Can't skip current TTS task." );
      mStopSpeakingSignal( StopSpeakingData() );
   }
}

signals::connection CWinTTS::onStartSpeaking( const api::tts::StartSpeakingSignal_t::slot_type& slot )
{
   return mStartSpeakingSignal.connect( slot );
}

signals::connection CWinTTS::onStopSpeaking( const api::tts::StopSpeakingSignal_t::slot_type& slot )
{
   return mStopSpeakingSignal.connect( slot );
}


void CWinTTS::enumerateVoices( void )
{
   assert( mVoices.empty() );
   assert( mLanguages.empty() );

   CComPtr<ISpObjectTokenCategory> cpCategory;
   CComPtr<IEnumSpObjectTokens> cpEnum = NULL;
   ULONG ulCount = 0;

   WINAPI_RUN_CHECKED( cpCategory.CoCreateInstance( CLSID_SpObjectTokenCategory ), "Can't initialize ISpObjectTokenCategory instance." );
   WINAPI_RUN_CHECKED( cpCategory->SetId( SPCAT_VOICES, false ), "Can't set ID SPCAT_VOICES for ISpObjectTokenCategory instance." );
   WINAPI_RUN_CHECKED( cpCategory->EnumTokens( NULL, NULL, &cpEnum ), "Can't initialize IEnumSpObjectTokens instance." );
   WINAPI_RUN_CHECKED( cpEnum->GetCount( &ulCount ), "Can't get number of voices from IEnumSpObjectTokens instance." );

   // Obtain a list of available voice tokens
   while ( ulCount-- )
   {
      ISpObjectToken* cpVoiceToken = NULL;
      WINAPI_RUN_CHECKED( cpEnum->Next( 1, &cpVoiceToken, NULL ), "Can't get next voice from IEnumSpObjectTokens instance." );
      assert( cpVoiceToken != NULL );
      SpVoiceTokenPtr voiceTokenPtr = SpVoiceToken::create( cpVoiceToken );
      std::string lang( voiceTokenPtr->getLanguage() );
      mVoices.insert( LangVoicePair( lang, voiceTokenPtr ) );
      mLanguages.insert( lang );
   }
}

#endif