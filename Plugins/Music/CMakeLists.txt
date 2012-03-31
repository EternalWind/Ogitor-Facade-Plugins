
project(MusicPlugin)

set(HEADER_FILES
        FaMusic.h
)

set(SOURCE_FILES
        FaMusic.cpp
)

include_directories(${OGITOR_INCLUDE_DIRS})
include_directories(.)

add_library(MusicPlugin SHARED ${HEADER_FILES} ${SOURCE_FILES})

set(LIBRARY_OUTPUT_PATH ${PLUGINS_OUTPUT_DIR})

set_target_properties(MusicPlugin PROPERTIES COMPILE_DEFINITIONS PLUGIN_EXPORT)
set_target_properties(MusicPlugin PROPERTIES SOLUTION_FOLDER Plugins)
