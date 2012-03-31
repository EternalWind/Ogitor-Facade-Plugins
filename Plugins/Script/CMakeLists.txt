
project(ScriptPlugin)

set(HEADER_FILES
        Scpath.h
)

set(SOURCE_FILES
        Scpath.cpp
)

include_directories(${OGITOR_INCLUDE_DIRS})
include_directories(.)

add_library(ScriptPlugin SHARED ${HEADER_FILES} ${SOURCE_FILES})

set(LIBRARY_OUTPUT_PATH ${PLUGINS_OUTPUT_DIR})

set_target_properties(ScriptPlugin PROPERTIES COMPILE_DEFINITIONS PLUGIN_EXPORT)
set_target_properties(ScriptPlugin PROPERTIES SOLUTION_FOLDER Plugins)
