
cmake_minimum_required (VERSION 3.8)
project (RSL2Solution)

# Whether or not to compile in tracy profiler
set(COMPILE_IN_PROFILER TRUE)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # using Visual Studio C++
    set(CMAKE_CXX_FLAGS "/experimental:external /external:anglebrackets /external:W0 /EHsc")
    add_compile_options(
        "$<$<CONFIG:Debug>:/Od>"
        "$<$<CONFIG:Release_WithDebugInfo>:/O2>"
        "$<$<CONFIG:Release>:/O2>"
    )
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
endif()

# Set C++ flags
set(CMAKE_CXX_STANDARD           20)
set(CMAKE_CXX_STANDARD_REQUIRED  ON)
set(CMAKE_CXX_EXTENSIONS         OFF)

# Set build output directory
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

message(BuildType=${CMAKE_BUILD_TYPE})
message(BinaryDir=${CMAKE_BINARY_DIR})

# Set config specific defines
if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    add_compile_definitions(DEBUG_BUILD)
endif()

# Include sub-projects.
add_subdirectory(Common) # Statically linked common lib used in multiple sub-projects
add_subdirectory(Profiler) # Profiler DLL included by other DLLs / applications which want to use tracy profiler. Needed for multi-DLL projects
add_subdirectory(Launcher) # Launcher that starts RFGR and injects the host DLL into it
add_subdirectory(Host) # Host DLL which is injected into RFGR and manages loading and reloading plugin DLLs
add_subdirectory (RSL2) # Main plugin DLL of RSL2 with function hooks and patches