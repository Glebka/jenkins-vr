file(GLOB_RECURSE sphinxbase_headers
    sphinxbase/include/sphinxbase/*.h
	sphinxbase/src/libsphinxbase/*.h
)

set(sphinxbase_headers
	${sphinxbase_headers}
	sphinxbase/include/win32/sphinx_config.h
    sphinxbase/include/win32/config.h
)

file(GLOB_RECURSE sphinxbase_src
    sphinxbase/src/libsphinxbase/*.c
)

set(sphinxbase_src
	${sphinxbase_src}
	sphinxbase/src/libsphinxad/ad_win32.c
)

include_directories(SYSTEM
	sphinxbase/include
	sphinxbase/include/win32
)

add_library(sphinxbase SHARED 
	${sphinxbase_headers}
	${sphinxbase_src}
)

target_link_libraries(sphinxbase winmm)

get_filename_component(_gst_libs_DIR ${GSTREAMER_LIBRARIES} DIRECTORY)
install(TARGETS sphinxbase DESTINATION ${_gst_libs_DIR}/gstreamer-1.0)