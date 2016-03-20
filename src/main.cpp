#include <iostream>
#include <gst/gst.h>

int main()
{
	gst_init( NULL, NULL );
	std::cout << "Hello world" << std::endl;
	return 0;
}