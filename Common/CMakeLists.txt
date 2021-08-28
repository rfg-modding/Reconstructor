cmake_minimum_required (VERSION 3.8)
project (Common VERSION 1.0.0
                  DESCRIPTION "Common"
                  LANGUAGES CXX)

# Make sure cmake doesn't add the build config name to the end of built DLLs
set(CMAKE_DEBUG_POSTFIX "")
set(CMAKE_RELEASE_POSTFIX "")
set(CMAKE_RELWITHDEBINFO_POSTFIX "")

# Get list of source files in project src folders recursively
# Places in SOURCES variable
file(GLOB_RECURSE SOURCES
    ${CMAKE_SOURCE_DIR}/Common/common/*.cpp
    ${CMAKE_SOURCE_DIR}/Common/common/*.h
)

# Add source to this project's static lib
add_library(Common STATIC ${SOURCES}
            ${CMAKE_SOURCE_DIR}/Dependencies/subhook/subhook.c)

target_link_libraries(Common PRIVATE subhook)

target_include_directories(Common SYSTEM PRIVATE
    ${CMAKE_SOURCE_DIR}/Common/
)