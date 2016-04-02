/*************************************************************************
 * jenkins-vr
 *************************************************************************
 * @file    CLogger.cpp
 * @date    02.04.16
 * @author  Hlieb Romanov
 * @brief
 ************************************************************************/
#include <sstream>
#include <map>

#include <boost/thread/lock_guard.hpp>
#include <boost/assign.hpp>

#include "../CLogger.hpp"

std::map<LogLevel::eLogLevel, std::string> CLogger::levelToStr 
   = boost::assign::map_list_of<LogLevel::eLogLevel, std::string>( LogLevel::LEVEL_DEBUG, "DEBUG" )
   ( LogLevel::LEVEL_INFO, "INFO" )
   ( LogLevel::LEVEL_WARNING, "WARN" )
   ( LogLevel::LEVEL_ERROR, "ERROR" )
   ( LogLevel::LEVEL_FATAL, "FATAL" );

CLogBuffer::LogBufferImpl::LogBufferImpl( LogLevel::eLogLevel _level )
   : level( _level )
{
}

CLogBuffer::LogBufferImpl::~LogBufferImpl( void )
{
   CLogger::print( level, stream.str() );
}

CLogBuffer::CLogBuffer( LogLevel::eLogLevel level )
   : mImpl( new LogBufferImpl( level ) )
{

}

CLogger::CLogger( void )
   : mConsoleLevel( LogLevel::LEVEL_INFO )
   , mFileLevel( LogLevel::LEVEL_DEBUG )
   , mConsoleStream( std::cout )
   , mFileStream()
{

}

CLogger::~CLogger( void )
{
   mFileStream.close();
}

void CLogger::setConsoleLogLevel( LogLevel::eLogLevel level )
{
   instance().do_setConsoleLogLevel( level );
}

void CLogger::setFileLogLevel( LogLevel::eLogLevel level )
{
   instance().do_setFileLogLevel( level );
}

void CLogger::setLogFile( const std::string& fileName )
{
   instance().do_setLogFile( fileName );
}

CLogBuffer CLogger::fatal( void )
{
   return instance().do_fatal();
}

CLogBuffer CLogger::error( void )
{
   return instance().do_error();
}

CLogBuffer CLogger::warning( void )
{
   return instance().do_warning();
}

CLogBuffer CLogger::info( void )
{
   return instance().do_info();
}

CLogBuffer CLogger::debug( void )
{
   return instance().do_debug();
}

CLogger& CLogger::instance( void )
{
   static boost::mutex instMutex;
   boost::lock_guard<boost::mutex> lock( instMutex );

   static CLogger logger;
   return logger;
}

void CLogger::print( LogLevel::eLogLevel level, const std::string& buffer )
{
   instance().do_print( level, buffer );
}

void CLogger::do_setConsoleLogLevel( LogLevel::eLogLevel level )
{
   boost::lock_guard<boost::mutex> lock( mLoggerMutex );
   mConsoleLevel = level;
}

void CLogger::do_setFileLogLevel( LogLevel::eLogLevel level )
{
   boost::lock_guard<boost::mutex> lock( mLoggerMutex );
   mFileLevel = level;
}

void CLogger::do_setLogFile( const std::string& fileName )
{
   boost::lock_guard<boost::mutex> lock( mLoggerMutex );
   if ( !mFileStream.is_open() )
   {
      mFileStream.open( fileName.c_str() );
   }
}

CLogBuffer CLogger::do_fatal( void )
{
   return CLogBuffer( LogLevel::LEVEL_FATAL );
}

CLogBuffer CLogger::do_error( void )
{
   return CLogBuffer( LogLevel::LEVEL_ERROR );
}

CLogBuffer CLogger::do_warning( void )
{
   return CLogBuffer( LogLevel::LEVEL_WARNING );
}

CLogBuffer CLogger::do_info( void )
{
   return CLogBuffer( LogLevel::LEVEL_INFO);
}

CLogBuffer CLogger::do_debug( void )
{
   return CLogBuffer( LogLevel::LEVEL_DEBUG );
}

void CLogger::do_print( LogLevel::eLogLevel level, const std::string& buffer )
{
   boost::lock_guard<boost::mutex> lock( mLoggerMutex );

   if ( mConsoleLevel <= level )
   {
      mConsoleStream << "[" << levelToStr[level] << "]\t\t" << buffer << std::endl;
   }

   if ( mFileStream.is_open() && mFileLevel <= level )
   {
      mFileStream << "[" << levelToStr[level] << "]\t\t" << buffer << std::endl;
   }
}