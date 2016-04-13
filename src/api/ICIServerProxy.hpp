/*************************************************************************
 * jenkins-vr
 *************************************************************************
 * @file    ICIServerProxy.hpp
 * @date    31.03.16
 * @author  Hlieb Romanov
 * @brief   
 ************************************************************************/
#pragma once

#include <string>
#include <boost/shared_ptr.hpp>

#include "ICIClient.hpp"
#include "IJob.hpp"

namespace api
{
   class ICIServerProxy;
   typedef boost::shared_ptr<ICIServerProxy> ServerProxyPtr;

   /**
    * More high-level abstraction of CI server
    */
   class ICIServerProxy
   {
   public:
      virtual ~ICIServerProxy( void ) = 0;
      virtual bool connect( const std::string& userName, const std::string& password ) = 0;
      virtual void disconnect( void ) = 0;
      virtual api::JobList getJobs( void ) const = 0;
      virtual api::JobPtr getJobByName( const std::string& jobName ) const = 0;
   };
}