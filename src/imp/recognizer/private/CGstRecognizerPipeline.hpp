/*************************************************************************
 * jenkins-vr
 *************************************************************************
 * @file    CGstRecognizerPipeline.hpp
 * @date    26.06.16
 * @author  Hlieb Romanov
 * @brief   Pipeline with pocketsphinx decoder
 ************************************************************************/
#pragma once

#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

#include "imp/gstreamer/CGstPipeline.hpp"

class CDecoder;
typedef boost::shared_ptr<CDecoder> DecoderPtr;

class CGstRecognizerPipeline
{
public:
   typedef boost::shared_ptr<GMainLoop> MainLoopPtr;

   /**
    * Constructs the pipeline with pocketsphinx element.
    * @param hmmDir - path to the directory with an acoustic model files.
    * @param dictFile - path to the pronunciation dictionary
    */
   CGstRecognizerPipeline( const std::string& hmmDir, const std::string& dictFile );
   ~CGstRecognizerPipeline( void );

   /**
    * Checks whether a pipeline in the READY state.
    */
   bool isInitialized( void );

   /**
    * Checks whether a pocketsphinx is listening to the aduio stream.
    */
   bool isListening( void );

   /**
    * Start listening.
    * This method sets the pipeline state to PLAYING.
    * @return true on success
    */
   bool startListening( void );

   /**
    * Stop listening.
    * This method sets the pipeline to the READY state.
    */
   void stopListening( void );

   /**
    * Get the underlying pocketsphinx decoder.
    */
   DecoderPtr getDecoder( void );

private:
   CGstRecognizerPipeline* self( void );
   bool initialize( void );
   void deinitialize( void );

   /**
    * Bus handler. Listens to the EOS message.
    */
   void onBusCall( GstBus* bus, GstMessage* msg );

private:
   bool mListening;
   bool mIsEosReceived;
   CGstPipeline mPipeline;
   CGstElement mPocketSphinx;
   MainLoopPtr mLoop;
   DecoderPtr mDecoder;
   boost::mutex mEosGuard;
   boost::condition_variable mEosCondition;
};