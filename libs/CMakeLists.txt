cmake_minimum_required(VERSION 3.0)
project(sphinx_vr)
include(ExternalProject)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING
  "Choose the type of build: Debug/Release" FORCE)
endif()

if(WIN32)
	include(sphinxbase.cmake)
	include(pocketsphinx.cmake)
	add_definitions(-D_USRDLL -DSPHINX_DLL -DSPHINXBASE_EXPORTS -DHAVE_CONFIG_H -D_CRT_SECURE_NO_DEPRECATE)
	add_definitions(-DSPHINX_DLL -DPOCKETSPHINX_EXPORTS)
	target_link_libraries(sphinxbase winmm)
else()
	message(STATUS "Adding external projects...")
	ExternalProject_Add(sphinxbase
		DOWNLOAD_COMMAND ""
		SOURCE_DIR sphinxbase
		CONFIGURE_COMMAND ${PROJECT_SOURCE_DIR}/sphinxbase/autogen.sh
		BUILD_COMMAND make
		BUILD_IN_SOURCE 1
	)
	ExternalProject_Add(pocketsphinx
		DOWNLOAD_COMMAND ""
		SOURCE_DIR pocketsphinx
		CONFIGURE_COMMAND "${PROJECT_SOURCE_DIR}/pocketsphinx/autogen.sh --with-sphinxbase=auto"
		BUILD_COMMAND make
		BUILD_IN_SOURCE 1
		DEPENDS sphinxbase
	)
endif()
