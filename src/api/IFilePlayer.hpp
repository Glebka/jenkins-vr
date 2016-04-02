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

namespace api
{
   class IFilePlayer;

   typedef boost::shared_ptr<IFilePlayer> FilePlayerPtr;

   class IFilePlayer
   {
      virtual ~IFilePlayer( void ) = 0;

      virtual bool startPlaying( const std::string& fileName ) = 0;
      virtual void stopPlaying( void ) = 0;
      virtual bool playFile( const std::string& fileName ) = 0;
      virtual bool isPlaying( void ) const = 0;
   };
}