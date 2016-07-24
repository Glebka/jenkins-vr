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
	ExternalProject_Add(sphinxbase_ext
		DOWNLOAD_COMMAND ""
		SOURCE_DIR ${PROJECT_SOURCE_DIR}/libs/sphinxbase
		BUILD_IN_SOURCE 1
		CONFIGURE_COMMAND ./configure --prefix=<INSTALL_DIR> --without-python --libdir=/usr/lib --includedir=/usr/include
		BUILD_COMMAND make
		INSTALL_COMMAND make install
	)
	ExternalProject_Add(pocketsphinx_ext
		DOWNLOAD_COMMAND ""
		SOURCE_DIR ${PROJECT_SOURCE_DIR}/libs/pocketsphinx
		BUILD_IN_SOURCE 1
		CONFIGURE_COMMAND ./configure --prefix=<INSTALL_DIR> --without-python --with-sphinxbase=auto --libdir=/usr/lib --includedir=/usr/include
		BUILD_COMMAND make
		INSTALL_COMMAND make install
		DEPENDS sphinxbase_ext
	)
	ExternalProject_Add(curl_ext
		DOWNLOAD_COMMAND ""
		SOURCE_DIR ${PROJECT_SOURCE_DIR}/libs/curl
		BUILD_IN_SOURCE 1
		CONFIGURE_COMMAND ./buildconf && ./configure --prefix=<INSTALL_DIR> --enable-http --libdir=/usr/lib --includedir=/usr/include
		BUILD_COMMAND make
		INSTALL_COMMAND make install
	)
endif()
