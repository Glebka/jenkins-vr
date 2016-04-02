/*************************************************************************
 * jenkins-vr
 *************************************************************************
 * @file    IJob.hpp
 * @date    02.04.16
 * @author  Hlieb Romanov
 * @brief
 ************************************************************************/
#pragma once
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/property_tree/ptree.hpp>

#include "ICIClient.hpp"

namespace api
{
   class IJob;

   typedef boost::shared_ptr<IJob> JobPtr;
   typedef std::vector<JobPtr> JobList;

   class IJob
   {
      virtual ~IJob( void ) = 0;

      virtual std::string getName( void ) const = 0;
      virtual std::string getStatus( void ) const = 0;
      virtual boost::property_tree::ptree getParamsSpec( void ) const = 0;
      virtual bool trigger( const boost::property_tree::ptree& params = boost::property_tree::ptree() ) = 0;
      virtual bool setState( api::JobState::eJobState state ) = 0;
      virtual api::JobState::eJobState getState( void ) const = 0;
   };
}