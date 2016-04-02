cmake_minimum_required(VERSION 3.0)

include(ExternalProject)

if(WIN32)
	include(sphinxbase.cmake)
	include(pocketsphinx.cmake)
	add_definitions(-D_USRDLL -DSPHINX_DLL -DSPHINXBASE_EXPORTS -DHAVE_CONFIG_H -D_CRT_SECURE_NO_DEPRECATE)
	add_definitions(-DSPHINX_DLL -DPOCKETSPHINX_EXPORTS)
	include(gstpocketsphinx.cmake)
	
	set(BUILD_CURL_EXE OFF CACHE BOOL "Set to ON to build cURL executable.")
	set(BUILD_CURL_TESTS OFF CACHE BOOL "Set to ON to build cURL tests.")
	set(HTTP_ONLY ON CACHE BOOL "disables all protocols except HTTP (This overrides all CURL_DISABLE_* options)")
	set(ENABLE_IPV6 OFF CACHE BOOL "Define if you want to enable IPv6 support")
	set(ENABLE_MANUAL OFF CACHE BOOL "to provide the built-in manual")
	add_subdirectory(curl)
	
else()
	message(STATUS "Adding external projects...")
	ExternalProject_Add(sphinxbase
		DOWNLOAD_COMMAND ""
		SOURCE_DIR sphinxbase
		CONFIGURE_COMMAND ${PROJECT_SOURCE_DIR}/libs/sphinxbase/autogen.sh --without-python --libdir=/usr/lib --includedir=/usr/include
		BUILD_COMMAND make
		INSTALL_COMMAND make install
		BUILD_IN_SOURCE 1
	)
	ExternalProject_Add(pocketsphinx
		DOWNLOAD_COMMAND ""
		SOURCE_DIR pocketsphinx
		CONFIGURE_COMMAND ${PROJECT_SOURCE_DIR}/libs/pocketsphinx/autogen.sh --without-python --with-sphinxbase=auto --libdir=/usr/lib --includedir=/usr/include
		BUILD_COMMAND make
		BUILD_IN_SOURCE 1
		INSTALL_COMMAND make install
		DEPENDS sphinxbase
	)
	ExternalProject_Add(curl
		DOWNLOAD_COMMAND ""
		SOURCE_DIR curl
		CONFIGURE_COMMAND ${PROJECT_SOURCE_DIR}/libs/curl/buildconf && ${PROJECT_SOURCE_DIR}/libs/curl/configure --enable-http --libdir=/usr/lib --includedir=/usr/include
		BUILD_COMMAND make
		BUILD_IN_SOURCE 1
		INSTALL_COMMAND make install
	)
endif()
