/*************************************************************************
 * jenkins-vr
 *************************************************************************
 * @file    IFilePlayer.hpp
 * @date    02.04.16
 * @author  Hlieb Romanov
 * @brief   Audio file player interface declaration
 ************************************************************************/
#pragma once

#include <string>
#include <boost/shared_ptr.hpp>

#include "Signals.hpp"

namespace api
{
   namespace player
   {
      class IFilePlayer; ///< Forward declaration of IFilePlayer interface

      typedef boost::shared_ptr<IFilePlayer> FilePlayerPtr;

      /**
       * StartPlaying signal data structure.
       * It is passed to the signal handler when the signal is emitted.
       * @sa StartPlayingSignal_t
       * @sa api::player::IFilePlayer::onStartPlaying()
       */
      struct StartPlayingData
      {
         std::string file;    ///< The file name to be played
         bool status;         ///< Status of the operation

         /**
          * Constructor
          * @param _file - full path to the file to be played
          * @param _status - status of the operation
          */
         StartPlayingData( const std::string& _file, bool _status )
            : file( _file )
            , status( _status )
         {

         }
      };

      /**
       * StopPlaying signal data structure.
       * It is passed to the signal handler when the signal is emitted.
       * @sa StopPlayingSignal_t
       * @sa api::player::IFilePlayer::onStopPlaying()
       */
      struct StopPlayingData
      {
         std::string file;

         StopPlayingData( const std::string& _file )
            : file( _file )
         {

         }
      };

      typedef signals::signal<void ( const StartPlayingData& e )> StartPlayingSignal_t; ///< StartPlaying signal type
      typedef signals::signal<void ( const StopPlayingData& e )> StopPlayingSignal_t;   ///< StopPlaying signal type

      /**
       * Simple file player interface class
       */
      class IFilePlayer
      {
      public:
         virtual ~IFilePlayer( void ) = 0 {}

         /**
          * Starts playing async. Returns immediately.
          * Emits StartPlaying signal.
          * @sa api::player::StartPlayingSignal_t
          * @sa onStartPlaying()
          * @return True - on succes, false - otherwise
          */
         virtual bool startPlaying( const std::string& fileName ) = 0;

         /**
          * Stops palying. Emits StopPlaying signal on completion.
          * @sa api::player::StopPlayingSignal_t
          * @sa onStopPlaying()
          */
         virtual void stopPlaying( void ) = 0;

         /**
          * Plays file syncronously. Returns after the end of playing.
          * Emits StartPlaying signal on start and StopPlaying on stop.
          * @sa api::player::StartPlayingSignal_t
          * @sa api::player::StopPlayingSignal_t
          * @return True - on success, false - otherwise
          */
         virtual bool playFile( const std::string& fileName ) = 0;

         /**
          * Checks the player state.
          */
         virtual bool isPlaying( void ) const = 0;

         /**
          * Registers handler for StartPlaying signal
          * @sa api::player::StartPlayingSignal_t
          * @return signal-slot connection
          */
         virtual signals::connection onStartPlaying( const StartPlayingSignal_t::slot_type& slot ) = 0;

         /**
          * Registers handler for StopPlaying signal
          * @sa api::player::StopPlayingSignal_t
          * @return signal-slot connection
          */
         virtual signals::connection onStopPlaying( const StopPlayingSignal_t::slot_type& slot ) = 0;
      };
   }
}