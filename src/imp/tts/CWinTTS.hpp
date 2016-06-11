/*************************************************************************
 * jenkins-vr
 *************************************************************************
 * @file    CWinTTS.hpp
 * @date    13.04.16
 * @author  Hlieb Romanov
 * @brief   Windows TTS class declaration
 ************************************************************************/
#pragma once

#ifdef _WIN32

#include <sapi.h>
#include <map>
#include <set>
#include <atlbase.h>
#include <atlconv.h>
extern CComModule _Module;
#include <atlcom.h>

#include "api/ITextToSpeech.hpp"

namespace details
{
   class SpVoiceToken;  //< Represents one TTS voice
   class ComContext;    //< COM context manager
   typedef boost::shared_ptr<SpVoiceToken> SpVoiceTokenPtr;
   typedef boost::shared_ptr<ComContext> ComContextPtr;
}

/**
 * Implementation of TTS API on Windows.
 * Uses Microsoft Speech API
 */
class CWinTTS : public api::tts::ITextToSpeech, boost::noncopyable
{
   typedef std::map<std::string, details::SpVoiceTokenPtr > VoicesMap;
   typedef VoicesMap::iterator VoicesIter;
   typedef std::pair<std::string, details::SpVoiceTokenPtr> LangVoicePair;

   CWinTTS( void );

public:
   /**
    * Factory method
    */
   static api::tts::TextToSpeechPtr create( void );

public:
   virtual ~CWinTTS( void );

   /**
    * @sa api::tts::ITextToSpeech::getAvailableLanguages()
    */
   virtual std::vector<std::string> getAvailableLanguages( void ) const;

   /**
    * @sa api::tts::ITextToSpeech::getCurrentLanguage()
    */
   virtual std::string getCurrentLanguage( void ) const;

   /**
    * @sa api::tts::ITextToSpeech::setLanguage()
    */
   virtual bool setLanguage( const std::string& language );

   /**
    * @sa api::tts::ITextToSpeech::sayAsync()
    */
   virtual bool sayAsync( const std::string& text );

   /**
    * @sa api::tts::ITextToSpeech::saySync()
    */
   virtual bool saySync( const std::string& text );

   /**
    * @sa api::tts::ITextToSpeech::isSpeaking()
    */
   virtual bool isSpeaking( void ) const;

   /**
    * @sa api::tts::ITextToSpeech::stopSpeaking()
    */
   virtual void stopSpeaking( void );

   /**
    * @sa api::tts::ITextToSpeech::onStartSpeaking()
    */
   virtual signals::connection onStartSpeaking( const api::tts::StartSpeakingSignal_t::slot_type& slot );

   /**
    * @sa api::tts::ITextToSpeech::onStopSpeaking()
    */
   virtual signals::connection onStopSpeaking( const api::tts::StopSpeakingSignal_t::slot_type& slot );

private:
   /**
    * Helper function for retrieving available voices list
    */
   void enumerateVoices( void );
private:
   details::ComContextPtr mComContext;
   ISpVoice* mVoice;
   VoicesMap mVoices;
   std::set<std::string> mLanguages;
   api::tts::StartSpeakingSignal_t mStartSpeakingSignal;
   api::tts::StopSpeakingSignal_t mStopSpeakingSignal;
};

#endif