
project(InteractorPlugin)

set(HEADER_FILES
        FaInteractor.h
)

set(SOURCE_FILES
        FaInteractor.cpp
)

include_directories(${OGITOR_INCLUDE_DIRS})
include_directories(.)

add_library(InteractorPlugin SHARED ${HEADER_FILES} ${SOURCE_FILES})

set(LIBRARY_OUTPUT_PATH ${PLUGINS_OUTPUT_DIR})

set_target_properties(InteractorPlugin PROPERTIES COMPILE_DEFINITIONS PLUGIN_EXPORT)
set_target_properties(InteractorPlugin PROPERTIES SOLUTION_FOLDER Plugins)