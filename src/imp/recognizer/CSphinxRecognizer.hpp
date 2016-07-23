/*************************************************************************
 * jenkins-vr
 *************************************************************************
 * @file    CSphinxRecognizer.hpp
 * @date    14.06.16
 * @author  Hlieb Romanov
 * @brief   Speech recognizer class declaration
 ************************************************************************/
#pragma once

#include <api/IRecognizer.hpp>

class CGstRecognizerPipeline;

class CSphinxRecognizer: public api::asr::IRecognizer, boost::noncopyable
{
   CSphinxRecognizer( void );

public:
   static api::asr::RecognizerPtr create( void );

public:

   virtual ~CSphinxRecognizer( void );

   /**
    * @sa api::asr::IRecognizer::getLanguage()
    */
   virtual std::string getLanguage( void ) const;

   /**
    * @sa api::asr::IRecognizer::setLanguage()
    */
   virtual void setLanguage( const std::string& language );

   /**
    * @sa api::asr::IRecognizer::getMode()
    */
   virtual api::asr::RecognizerMode::eRecognizerMode getMode( void ) const;

   /**
    * @sa api::asr::IRecognizer::setMode()
    */
   virtual bool setMode( api::asr::RecognizerMode::eRecognizerMode mode );

   /**
    * @sa api::asr::IRecognizer::listen()
    */
   virtual bool listen( void );

   /**
    * @sa api::asr::IRecognizer::isListening()
    */
   virtual bool isListening( void ) const;

   /**
    * @sa api::asr::IRecognizer::stopListening()
    */
   virtual void stopListening( void );

   /**
    * @sa api::asr::IRecognizer::phonetize()
    */
   virtual bool phonetize( const std::string& group, const api::asr::GraphemePhonemeList& g2pList );

   /**
    * @sa api::asr::IRecognizer::onStartListening()
    */
   virtual signals::connection onStartListening( const api::asr::StartListeningSignal_t::slot_type& slot );

   /**
    * @sa api::asr::IRecognizer::onRecognitionResult()
    */
   virtual signals::connection onRecognitionResult( const api::asr::RecognitionResultSignal_t::slot_type& slot );

   /**
    * @sa api::asr::IRecognizer::onStopListening()
    */
   virtual signals::connection onStopListening( const api::asr::StopListeningSignal_t::slot_type& slot );
private:

   void reinit( void );

private:
   typedef boost::shared_ptr<CGstRecognizerPipeline> GstRecognizerPipelinePtr;
   std::string mLanguage;
   api::asr::RecognizerMode::eRecognizerMode mMode;
   GstRecognizerPipelinePtr mRecognizerPipeline;
   api::asr::StartListeningSignal_t mStartListening;
   api::asr::StopListeningSignal_t mStopListening;
   api::asr::RecognitionResultSignal_t mRecognitionResult;
};
