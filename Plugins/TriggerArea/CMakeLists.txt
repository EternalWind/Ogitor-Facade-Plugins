
project(TriggerAreaPlugin)

set(HEADER_FILES
        FaTriggerArea.h
)

set(SOURCE_FILES
        FaTriggerArea.cpp
)

include_directories(${OGITOR_INCLUDE_DIRS})
include_directories(.)

add_library(TriggerAreaPlugin SHARED ${HEADER_FILES} ${SOURCE_FILES})

set(LIBRARY_OUTPUT_PATH ${PLUGINS_OUTPUT_DIR})

set_target_properties(TriggerAreaPlugin PROPERTIES COMPILE_DEFINITIONS PLUGIN_EXPORT)
set_target_properties(TriggerAreaPlugin PROPERTIES SOLUTION_FOLDER Plugins)
