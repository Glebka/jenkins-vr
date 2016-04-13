/*************************************************************************
 * jenkins-vr
 *************************************************************************
 * @file    CLogger.hpp
 * @date    02.04.16
 * @author  Hlieb Romanov
 * @brief
 ************************************************************************/
#pragma once

#include <string>
#include <iostream>
#include <ostream>
#include <fstream>
#include <map>

#include <boost/noncopyable.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/shared_ptr.hpp>

namespace LogLevel
{
   enum eLogLevel
   {
      LEVEL_DEBUG,
      LEVEL_INFO,
      LEVEL_WARNING,
      LEVEL_ERROR,
      LEVEL_FATAL
   };
}

class CLogBuffer
{
public:
   struct LogBufferImpl
   {
      std::stringstream stream;
      LogLevel::eLogLevel level;

      LogBufferImpl( LogLevel::eLogLevel _level = LogLevel::LEVEL_DEBUG );
      ~LogBufferImpl( void );
   };

   typedef boost::shared_ptr<LogBufferImpl> LogBufferImplPtr;
public:
   explicit CLogBuffer( LogLevel::eLogLevel level = LogLevel::LEVEL_DEBUG );

   template <typename T>
   CLogBuffer& operator<< ( T val )
   {
      mImpl->stream << val;
      return *this;
   }

private:
   LogBufferImplPtr mImpl;
};

class CLogger : public boost::noncopyable
{
   friend struct CLogBuffer::LogBufferImpl;

public:
   CLogger( void );
   virtual ~CLogger( void );

public:

   static void setConsoleLogLevel( LogLevel::eLogLevel level );
   static void setFileLogLevel( LogLevel::eLogLevel level );
   static void setLogFile( const std::string& fileName );

   static CLogBuffer fatal( void );
   static CLogBuffer error( void );
   static CLogBuffer warning( void );
   static CLogBuffer info( void );
   static CLogBuffer debug( void );

private:

   static CLogger& instance( void );
   static void print( LogLevel::eLogLevel level, const std::string& buffer );

private:

   void do_setConsoleLogLevel( LogLevel::eLogLevel level );
   void do_setFileLogLevel( LogLevel::eLogLevel level );
   void do_setLogFile( const std::string& fileName );

   CLogBuffer do_fatal( void );
   CLogBuffer do_error( void );
   CLogBuffer do_warning( void );
   CLogBuffer do_info( void );
   CLogBuffer do_debug( void );
   void do_print( LogLevel::eLogLevel level, const std::string& buffer );

private:
   LogLevel::eLogLevel mConsoleLevel;
   LogLevel::eLogLevel mFileLevel;
   std::ostream& mConsoleStream;
   std::ofstream mFileStream;
   boost::mutex mLoggerMutex;

private:   
   static std::map<LogLevel::eLogLevel, std::string> levelToStr;
};