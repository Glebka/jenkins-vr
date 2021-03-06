cmake_minimum_required(VERSION 3.1.3)

project(jenkins-vr)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

find_package(GStreamer 1.6.4 REQUIRED)
find_package(GLIB2 REQUIRED)

set(Boost_USE_STATIC_LIBS        ON CACHE BOOL "")
set(Boost_USE_MULTITHREADED      ON CACHE BOOL "")
set(Boost_USE_STATIC_RUNTIME    OFF CACHE BOOL "") 

find_package(Boost REQUIRED COMPONENTS system date_time chrono thread program_options regex)

include_directories(SYSTEM
   ${GSTREAMER_INCLUDE_DIRS}
   ${Boost_INCLUDE_DIRS}
   ${GLIB2_INCLUDE_DIRS}
   libs/pocketsphinx/include
   libs/sphinxbase/include
)

if(WIN32)
	include_directories(SYSTEM
		libs/sphinxbase/include/win32
	)
endif()

include_directories(
   src
)

add_subdirectory(libs)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING
  "Choose the type of build: Debug/Release" FORCE)
endif()

if(NOT WIN32)
	include(CheckCXXCompilerFlag)
	CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
	CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
	if(COMPILER_SUPPORTS_CXX11)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
	elseif(COMPILER_SUPPORTS_CXX0X)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
	else()
			message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
	endif()
endif()

file(GLOB_RECURSE headers
   src/*.h
   src/*.hpp
)

file(GLOB_RECURSE sources
   src/*.c
   src/*.cpp
)

add_executable(jenkins-vr 
   ${headers}
   ${sources}
)

if(NOT WIN32)
	add_dependencies(jenkins-vr sphinxbase_ext pocketsphinx_ext)
	include_directories(SYSTEM
		usr/include/sphinxbase
		usr/include/pocketsphinx
		usr/local/include/sphinxbase
		usr/local/include/pocketsphinx
	)
	link_directories(
		/lib
		/usr/lib
		/usr/local/lib
		/usr/lib64
		/usr/lib/x86_64-linux-gnu
	)
endif()

message(STATUS "GST LIBS: ${GSTREAMER_LIBRARIES}")
message(STATUS "GLIB2 LIBS: ${GLIB2_LIBRARIES}")
message(STATUS "BOOST LIBS: ${Boost_LIBRARIES}")

target_link_libraries(jenkins-vr
   ${GSTREAMER_LIBRARIES}
   ${GLIB2_LIBRARIES}
   ${Boost_LIBRARIES}
   sphinxbase
   pocketsphinx
)

if(WIN32)
   target_link_libraries(jenkins-vr
      sapi.lib
   )
endif()
