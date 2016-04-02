/*************************************************************************
 * jenkins-vr
 *************************************************************************
 * @file    ICIClient.hpp
 * @date    27.03.16
 * @author  Hlieb Romanov
 * @brief   
 ************************************************************************/
#pragma once

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/property_tree/ptree.hpp>

namespace api
{
   namespace JobState
   {
      enum eJobState
      {
         JOB_DISABLED,
         JOB_ENABLED
      };
   }

   class ICIClient;

   typedef boost::shared_ptr<ICIClient> CIClientPtr;

   /**
    * Interface for CI server client implementations:
    * http(s), telnet, ssh, etc.
    */
   class ICIClient
   {
      virtual ~ICIClient( void ) = 0;

      /**
       * Connect to CI server
       * @return true - on success
       */
      virtual bool connect( const std::string& userName, const std::string& password ) = 0;

      /**
       * Disconnect form CI server
       */
      virtual void disconnect( void ) = 0;

      /**
       * Get CI server version
       */
      virtual std::string getServerVersion( void ) = 0;

      /**
       * Get job names on the CI server
       */
      virtual std::vector<std::string> getJobsList( void ) = 0;

      /**
       * Get job status as string by job name
       */
      virtual std::string getJobStatus( const std::string& jobName ) = 0;

      /**
       * Get job params specification as property tree
       */
      virtual boost::property_tree::ptree getJobParamsSpec( const std::string& jobName ) = 0;

      /**
       * Trigger job on CI server with params
       * @return true - if job was triggered
       */
      virtual bool triggerJob( const std::string& jobName, const boost::property_tree::ptree& params = boost::property_tree::ptree() ) = 0;

      /**
       * Enable or disable job on CI server
       * @return true - on success
       */
      virtual bool setJobState( JobState::eJobState state ) = 0;
   };
}