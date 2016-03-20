cmake_minimum_required(VERSION 3.0)

project(helloworld)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

find_package(GStreamer 1.6.3 REQUIRED)
find_package(GLIB2 REQUIRED)

include_directories(SYSTEM
	${GSTREAMER_INCLUDE_DIRS}
	${GLIB2_INCLUDE_DIRS}
)

set(headers
)

set(sources
	src/main.cpp
)
message(STATUS ${GSTREAMER_LIBRARIES})
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING
  "Choose the type of build: Debug/Release" FORCE)
endif()

add_executable(helloworld ${sources})
target_link_libraries(helloworld
	${GSTREAMER_LIBRARIES}
	${GLIB2_LIBRARIES}
)
