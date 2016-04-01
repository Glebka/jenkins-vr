/*************************************************************************
 * jenkins-vr
 *************************************************************************
 * @file    ICIServerProxy.hpp
 * @date    31.03.16
 * @author  Hlieb Romanov
 * @brief   
 ************************************************************************/
#pragma once

#include "ICIClient.hpp"

namespace api
{
   /**
    * More high-level abstraction of CI server
    */
   class ICIServerProxy
   {
      virtual ~ICIServerProxy( void ) = 0;
      virtual bool connect( const std::string& userName, const std::string& password );

   };
}