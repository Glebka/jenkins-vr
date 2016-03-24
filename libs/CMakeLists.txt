cmake_minimum_required(VERSION 3.0)
project(sphinx_vr)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING
  "Choose the type of build: Debug/Release" FORCE)
endif()

include(sphinxbase.cmake)
include(pocketsphinx.cmake)

if(WIN32)
	add_definitions(-D_USRDLL -DSPHINX_DLL -DSPHINXBASE_EXPORTS -DHAVE_CONFIG_H -D_CRT_SECURE_NO_DEPRECATE)
	add_definitions(-DSPHINX_DLL -DPOCKETSPHINX_EXPORTS)
	target_link_libraries(sphinxbase winmm)
endif()