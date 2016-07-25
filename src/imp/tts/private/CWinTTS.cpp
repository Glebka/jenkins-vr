/*************************************************************************
 * jenkins-vr
 *************************************************************************
 * @file    CWinTTS.cpp
 * @date    13.04.16
 * @author  Hlieb Romanov
 * @brief   Windows TTS class implementation
 ************************************************************************/
#ifdef _WIN32

#include <tchar.h>
#include <sstream>
#include <boost/format.hpp>
#include <boost/thread.hpp>

#include "imp/tts/CWinTTS.hpp"
#include "imp/logger/CLogger.hpp"

using namespace api::tts;
using namespace details;

static const int MAX_WAIT_TIME_MS = 2 * 60 * 100;

typedef boost::shared_ptr<ISpDataKey> ISpDataKeyPtr;
typedef boost::shared_ptr<ISpObjectTokenCategory> ISpObjectTokenCategoryPtr;
typedef boost::shared_ptr<IEnumSpObjectTokens> IEnumSpObjectTokensPtr;

static void comDeleter( IUnknown* object )
{
   object->Release();
}

static std::string LPWSTR2string( LPWSTR wstring )
{
   int bufferSize = WideCharToMultiByte( CP_UTF8, 0, wstring, wcslen( wstring ), NULL, 0, NULL, NULL );
   std::string buffer( bufferSize, 0 );
   if ( WideCharToMultiByte( CP_UTF8, 0, wstring, wcslen( wstring ), 
      const_cast<char*>( buffer.c_str() ), bufferSize, NULL, NULL ) == 0 )
   {
      std::string errorMessage( "Unable to convert UNICODE string to UTF-8." );
      CLogger::fatal() << errorMessage;
      throw std::runtime_error( errorMessage );
   }
   return buffer;
}

static std::wstring string2Wstring( const std::string& str )
{
   int wChars = MultiByteToWideChar( CP_UTF8, 0, str.c_str(), str.size(), NULL, 0 );
   std::wstring buffer( wChars, 0 );
   if ( MultiByteToWideChar( CP_UTF8, 0, str.c_str(), 
         str.size(), const_cast<wchar_t*>( buffer.c_str() ), wChars ) == 0 )
   {
      std::string errorMessage( "Unable to convert UTF-8 string to UNICODE." );
      CLogger::fatal() << errorMessage;
      throw std::runtime_error( errorMessage );
   }
   return buffer;
}

static void WINAPI_RUN_CHECKED( HRESULT winApiCallResult, const std::string& errorMessage )
{
   if ( FAILED( winApiCallResult ) )
   {
      CLogger::fatal() << errorMessage;
      throw std::runtime_error( errorMessage );
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
         ISpDataKey* rpAttrs = NULL;
         ISpDataKeyPtr pAttrs;

         WINAPI_RUN_CHECKED( mVoiceToken->OpenKey( L"Attributes", &rpAttrs ), "Can't get Attributes key from voice description." );
         pAttrs.reset( rpAttrs, comDeleter );
         WINAPI_RUN_CHECKED( pAttrs->GetStringValue( L"Name", &name ), "Can't get language code for voice" );
         return LPWSTR2string( name );
      }

      std::string getLanguage( void )
      {
         assert( mVoiceToken != NULL );

         const int MAX_LOCALE_LEN = 19;
         const int MAX_LOCALE_PART_LEN = 9;

         LPWSTR langCode = NULL;
         ISpDataKey* rpAttrs = NULL;
         ISpDataKeyPtr pAttrs;

         WINAPI_RUN_CHECKED( mVoiceToken->OpenKey( L"Attributes", &rpAttrs ), "Can't get Attributes key from voice description." );
         pAttrs.reset( rpAttrs, comDeleter );
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
   std::wstring uniText = string2Wstring( text );

   HRESULT hr = mVoice->Speak( uniText.c_str(), SPF_PURGEBEFORESPEAK | SPF_ASYNC, NULL );
   if ( !FAILED( hr ) )
   {
      HANDLE waitHandle = mVoice->SpeakCompleteEvent();
      boost::thread notifier = boost::thread([this,waitHandle]( void ) {
         if ( WaitForSingleObject( waitHandle, MAX_WAIT_TIME_MS ) != WAIT_OBJECT_0 )
         {
            std::string message( "Can't initialize COM instance." );
            CLogger::fatal() << message;
            throw std::runtime_error( message );
         }
         mStopSpeakingSignal( StopSpeakingData() );
      });
      notifier.detach();
   }
   mStartSpeakingSignal( StartSpeakingData( text, !FAILED( hr ) ) );
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

   ISpObjectTokenCategoryPtr cpCategory;
   ISpObjectTokenCategory* rcpCategory = NULL;
   IEnumSpObjectTokensPtr cpEnum;
   IEnumSpObjectTokens* rcpEnum = NULL;
   ULONG ulCount = 0;

   WINAPI_RUN_CHECKED( CoCreateInstance(CLSID_SpObjectTokenCategory, 
      NULL, CLSCTX_INPROC_SERVER, IID_ISpObjectTokenCategory, (void**)&rcpCategory), "Can't initialize ISpObjectTokenCategory instance." );

   cpCategory.reset( rcpCategory, comDeleter );

   WINAPI_RUN_CHECKED( cpCategory->SetId( SPCAT_VOICES, false ), "Can't set ID SPCAT_VOICES for ISpObjectTokenCategory instance." );
   WINAPI_RUN_CHECKED( cpCategory->EnumTokens( NULL, NULL, &rcpEnum ), "Can't initialize IEnumSpObjectTokens instance." );

   cpEnum.reset( rcpEnum, comDeleter );

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