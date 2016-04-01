cmake_minimum_required(VERSION 3.0)

project(jenkins-vr)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

find_package(GStreamer 1.6.3 REQUIRED)
find_package(GLIB2 REQUIRED)
find_package(Boost REQUIRED COMPONENTS system date_time thread program_options regex)

include_directories(SYSTEM
   ${GSTREAMER_INCLUDE_DIRS}
   ${Boost_INCLUDE_DIRS}
   ${GLIB2_INCLUDE_DIRS}
)

add_subdirectory(libs)

message(STATUS ${GSTREAMER_LIBRARIES})
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING
  "Choose the type of build: Debug/Release" FORCE)
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

target_link_libraries(jenkins-vr
   ${GSTREAMER_LIBRARIES}
   ${GLIB2_LIBRARIES}
)
