
cmake_minimum_required (VERSION 3.8)
project (Profiler VERSION 1.0.0
                  DESCRIPTION "Profiler"
                  LANGUAGES CXX)

# Make the output dll name always be Profiler.dll. Cmake tries to add config type (_debug, release_withdebuginfo) to end
set(CMAKE_DEBUG_POSTFIX "")
set(CMAKE_RELEASE_POSTFIX "")
set(CMAKE_RELWITHDEBINFO_POSTFIX "")

# Get list of source files in project src folders recursively
# Places in SOURCES variable
file(GLOB_RECURSE SOURCES 
    ${CMAKE_SOURCE_DIR}/Profiler/profiler/*.cpp
    ${CMAKE_SOURCE_DIR}/Profiler/profiler/*.h
)

# Add source to this project's DLL
add_library(Profiler SHARED ${SOURCES}
            # Include tracy client file
            ${CMAKE_SOURCE_DIR}/dependencies/tracy/TracyClient.cpp
            )

# Add include directories
target_include_directories(Profiler SYSTEM PRIVATE
    ${CMAKE_SOURCE_DIR}/Common/
    ${CMAKE_SOURCE_DIR}/Profiler/
)

add_compile_definitions(TRACY_EXPORTS)