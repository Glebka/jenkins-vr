/*************************************************************************
 * jenkins-vr
 *************************************************************************
 * @file    IFilePlayer.hpp
 * @date    02.04.16
 * @author  Hlieb Romanov
 * @brief
 ************************************************************************/
#pragma once

#include <string>
#include <boost/shared_ptr.hpp>

#include "Signals.hpp"

namespace api
{
   class IFilePlayer;

   typedef boost::shared_ptr<IFilePlayer> FilePlayerPtr;

   struct StartPlayingData
   {
      std::string file;
      bool status;

      StartPlayingData( const std::string& _file, bool _status )
         : file( _file )
         , status( status )
      {

      }
   };

   struct StopPlayingData
   {
      std::string file;
      StopPlayingData( const std::string& _file )
         : file( _file )
      {

      }
   };

   typedef signals::signal<void ( const StartPlayingData& e )> StartPlaying;
   typedef signals::signal<void ( const StopPlayingData& e )> StopPlaying;

   class IFilePlayer
   {
   public:
      virtual ~IFilePlayer( void ) = 0;

      virtual bool startPlaying( const std::string& fileName ) = 0;
      virtual void stopPlaying( void ) = 0;
      virtual bool playFile( const std::string& fileName ) = 0;
      virtual bool isPlaying( void ) const = 0;
   };
}