/*************************************************************************
 * jenkins-vr
 *************************************************************************
 * @file    CFilePlayer.hpp
 * @date    12.06.16
 * @author  Hlieb Romanov
 * @brief   File player class declaration
 ************************************************************************/
#pragma once

#include <boost/thread.hpp>
#include <api/IFilePlayer.hpp>

class CFilePlayer: public api::player::IFilePlayer, boost::noncopyable
{
   CFilePlayer( void );

public:
   /**
    * Factory method
    */
   static api::player::FilePlayerPtr create( void );

public:

   virtual ~CFilePlayer( void );

   /**
    * @sa api::player::IFilePlayer::startPlaying()
    */
   virtual bool startPlaying( const std::string& fileName );

   /**
    * @sa api::player::IFilePlayer::stopPlaying()
    */
   virtual void stopPlaying( void );

   /**
    * @sa api::player::IFilePlayer::playFile()
    */
   virtual bool playFile( const std::string& fileName );

   /**
    * @sa api::player::IFilePlayer::isPlaying()
    */
   virtual bool isPlaying( void ) const;

   /**
    * @sa api::player::IFilePlayer::onStartPlaying()
    */
   virtual signals::connection onStartPlaying( const api::player::StartPlayingSignal_t::slot_type& slot );

   /**
    * @sa api::player::IFilePlayer::onStopPlaying()
    */
   virtual signals::connection onStopPlaying( const api::player::StopPlayingSignal_t::slot_type& slot );

private:
   class CGstPlayerPipeline;
   typedef boost::shared_ptr<CGstPlayerPipeline> GstPlayerPipelinePtr;

   GstPlayerPipelinePtr mPlayerPipeline;
   api::player::StartPlayingSignal_t mStartPlaying;
   api::player::StopPlayingSignal_t mStopPlaying;
   boost::thread mNotifierThread;
};