file(GLOB_RECURSE pocketsphinx_headers
    pocketsphinx/include/*.h
	pocketsphinx/src/libpocketsphinx/*.h
)

file(GLOB_RECURSE pocketsphinx_src
	pocketsphinx/src/libpocketsphinx/*.c
)

include_directories(SYSTEM
	pocketsphinx/include
)

set (apply_cmd "apply_patch.bat")
execute_process(COMMAND ${apply_cmd}
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  RESULT_VARIABLE git_result
  OUTPUT_VARIABLE git_ver)

add_library(pocketsphinx SHARED
	${pocketsphinx_headers}
	${pocketsphinx_src}
)

target_link_libraries(pocketsphinx sphinxbase)
get_filename_component(_gst_libs_DIR ${GSTREAMER_LIBRARIES} DIRECTORY)
install(TARGETS pocketsphinx DESTINATION ${_gst_libs_DIR}/gstreamer-1.0)