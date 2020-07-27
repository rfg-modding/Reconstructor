cmake_minimum_required (VERSION 3.8)
project (GuiBase VERSION 1.0.0
                  DESCRIPTION "GuiBase"
                  LANGUAGES CXX)

# Make sure cmake doesn't add the build config name to the end of built DLLs (eg. GuiBase_debug.dll instead of GuiBase.dll)
set(CMAKE_DEBUG_POSTFIX "")
set(CMAKE_RELEASE_POSTFIX "")
set(CMAKE_RELWITHDEBINFO_POSTFIX "")

# Get list of source files in project src folders recursively
# Places in SOURCES variable
file(GLOB_RECURSE SOURCES 
    ${CMAKE_SOURCE_DIR}/GuiBase/guiBase/*.cpp
    ${CMAKE_SOURCE_DIR}/GuiBase/guiBase/*.h
)

# Add source to this project's DLL
add_library(GuiBase SHARED ${SOURCES})

# Compile in profiler if it's flag is set
if(COMPILE_IN_PROFILER)
    target_link_libraries(GuiBase PRIVATE Profiler)
endif()

# Add include directories
target_include_directories(GuiBase SYSTEM PRIVATE
    ${CMAKE_SOURCE_DIR}/Common/
    ${CMAKE_SOURCE_DIR}/GuiBase/
    ${CMAKE_SOURCE_DIR}/dependencies/
)

add_compile_definitions(TRACY_IMPORTS)