#include <iostream>
#include <gst/gst.h>

#include "imp/logger/CLogger.hpp"

int main()
{
	gst_init( NULL, NULL );

   CLogger::fatal() << "Hello fatal! " << "This is on the same line.";
   CLogger::error() << "Hello error! " << "This is on the same line.";
   CLogger::warning() << "Hello warning! " << "This is on the same line.";
   CLogger::info() << "Hello info! " << "This is on the same line.";
   CLogger::debug() << "Hello debug! " << "This is on the same line.";

	return 0;
}