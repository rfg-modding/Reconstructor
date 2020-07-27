cmake_minimum_required (VERSION 3.8)
project (Host VERSION 1.0.0
                  DESCRIPTION "Host"
                  LANGUAGES CXX)

# Make sure cmake doesn't add the build config name to the end of built DLLs (eg. Host_debug.dll instead of Host.dll)
set(CMAKE_DEBUG_POSTFIX "")
set(CMAKE_RELEASE_POSTFIX "")
set(CMAKE_RELWITHDEBINFO_POSTFIX "")

# Get list of source files in project src folders recursively
# Places in SOURCES variable
file(GLOB_RECURSE SOURCES 
    ${CMAKE_SOURCE_DIR}/Host/host/*.cpp
    ${CMAKE_SOURCE_DIR}/Host/host/*.h
)

# Add source to this project's DLL
add_library(Host SHARED ${SOURCES})

target_link_libraries(Host PRIVATE Common)

# Compile in profiler if it's flag is set
if(COMPILE_IN_PROFILER)
    target_link_libraries(Host PRIVATE Profiler)
endif()

# Add include directories
target_include_directories(Host SYSTEM PRIVATE
    ${CMAKE_SOURCE_DIR}/Common/
    ${CMAKE_SOURCE_DIR}/Host/
    ${CMAKE_SOURCE_DIR}/dependencies/
)

add_compile_definitions(TRACY_IMPORTS)