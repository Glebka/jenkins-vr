file(GLOB_RECURSE sphinxbase_headers
    sphinxbase/include/sphinxbase/*.h
	sphinxbase/src/libsphinxbase/*.h
)

if(WIN32)
set(sphinxbase_headers
	${sphinxbase_headers}
	sphinxbase/include/win32/sphinx_config.h
    sphinxbase/include/win32/config.h
)
else()
#TODO: insert config.h generation
endif()

file(GLOB_RECURSE sphinxbase_src
    sphinxbase/src/libsphinxbase/*.c
)

if(WIN32)
	set(sphinxbase_src
		${sphinxbase_src}
		sphinxbase/src/libsphinxad/ad_win32.c
	)
else()
	set(sphinxbase_src
		${sphinxbase_src}
		sphinxbase/src/libsphinxad/ad_alsa.c
	)
endif()

include_directories(SYSTEM
	sphinxbase/include
	sphinxbase/include/win32
)

add_library(sphinxbase SHARED 
	${sphinxbase_headers}
	${sphinxbase_src}
)