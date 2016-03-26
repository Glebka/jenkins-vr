include_directories(SYSTEM
	${GSTREAMER_BASE_INCLUDE_DIRS}
	${GSTREAMER_AUDIO_INCLUDE_DIRS}
)

set(gst_plugin_path
	pocketsphinx/src/gst-plugin
)

set(gstpocketsphinx_sources
	${gst_plugin_path}/gstpocketsphinx.h
	${gst_plugin_path}/gstpocketsphinx.c
)

add_library(gstpocketsphinx SHARED
	${gstpocketsphinx_sources}
)

target_link_libraries(gstpocketsphinx
	sphinxbase
	pocketsphinx
	${GSTREAMER_AUDIO_LIBRARIES}
	${GSTREAMER_BASE_LIBRARIES}
	${GSTREAMER_LIBRARIES}
	${GLIB2_LIBRARIES}
)

get_filename_component(_gst_libs_DIR ${GSTREAMER_LIBRARIES} DIRECTORY)
install(TARGETS gstpocketsphinx DESTINATION ${_gst_libs_DIR}/gstreamer-1.0)