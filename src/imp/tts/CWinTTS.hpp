/*************************************************************************
 * jenkins-vr
 *************************************************************************
 * @file    CWinTTS.hpp
 * @date    13.04.16
 * @author  Hlieb Romanov
 * @brief
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

class SpVoiceToken;
class ComContext;
typedef boost::shared_ptr<SpVoiceToken> SpVoiceTokenPtr;
typedef boost::shared_ptr<ComContext> ComContextPtr;

class CWinTTS : public api::ITextToSpeech, boost::noncopyable
{
   CWinTTS( void );
public:
   typedef std::map<std::string, SpVoiceTokenPtr > VoicesMap;
   typedef VoicesMap::iterator VoicesIter;
   typedef std::pair<std::string, SpVoiceTokenPtr> LangVoicePair;

   static api::TextToSpeechPtr create( void );
public:
   virtual ~CWinTTS( void );

   virtual std::vector<std::string> getAvailableLanguages( void ) const;
   virtual std::string getCurrentLanguage( void ) const;
   virtual bool setLanguage( const std::string& language );
   virtual bool sayAsync( const std::string& text );
   virtual bool saySync( const std::string& text );
   virtual bool isSpeaking( void ) const;
   virtual void stopSpeaking( void );

   virtual signals::connection subscribe( const api::StartSpeaking::slot_type& slot );
   virtual signals::connection subscribe( const api::StopSpeaking::slot_type& slot );

private:
   void enumerateVoices( void );
private:
   ComContextPtr mComContext;
   ISpVoice* mVoice;
   VoicesMap mVoices;
   std::set<std::string> mLanguages;
   api::StartSpeaking mStartSpeakingSignal;
   api::StopSpeaking mStopSpeakingSignal;
};

#endif