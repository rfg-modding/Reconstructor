cmake_minimum_required (VERSION 3.8)
project (Reconstructor VERSION 1.0.0
                  DESCRIPTION "Reconstructor"
                  LANGUAGES CXX)

# Make sure cmake doesn't add the build config name to the end of built DLLs (eg. Reconstructor_debug.dll instead of Reconstructor.dll)
set(CMAKE_DEBUG_POSTFIX "")
set(CMAKE_RELEASE_POSTFIX "")
set(CMAKE_RELWITHDEBINFO_POSTFIX "")

# Get list of source files in project src folders recursively
# Places in SOURCES variable
file(GLOB_RECURSE SOURCES
    ${CMAKE_SOURCE_DIR}/Reconstructor/reconstructor/*.cpp
    ${CMAKE_SOURCE_DIR}/Reconstructor/reconstructor/*.h
)

add_compile_definitions(IMGUI_IMPL_WIN32_DISABLE_LINKING_XINPUT IMGUI_IMPL_WIN32_DISABLE_GAMEPAD)

# Add source to this project's DLL
add_library(Reconstructor SHARED ${SOURCES}
            ${CMAKE_SOURCE_DIR}/Dependencies/subhook/subhook.c
            ${CMAKE_SOURCE_DIR}/Dependencies/imgui/imgui.cpp
            ${CMAKE_SOURCE_DIR}/Dependencies/imgui/imgui_demo.cpp
            ${CMAKE_SOURCE_DIR}/Dependencies/imgui/imgui_draw.cpp
            ${CMAKE_SOURCE_DIR}/Dependencies/imgui/imgui_widgets.cpp
            ${CMAKE_SOURCE_DIR}/Dependencies/imgui/imgui_tables.cpp
            ${CMAKE_SOURCE_DIR}/Dependencies/imgui/backends/imgui_impl_win32.cpp
            )

target_link_libraries(Reconstructor PRIVATE subhook)
target_link_libraries(Reconstructor PRIVATE AsmJit::AsmJit)
target_link_libraries(Reconstructor PRIVATE RFGR_Types)
target_link_libraries(Reconstructor PRIVATE Common)

# Compile in profiler if it's flag is set
if(COMPILE_IN_PROFILER)
    target_link_libraries(Reconstructor PRIVATE Profiler)
endif()

# Add include directories
target_include_directories(Reconstructor SYSTEM PRIVATE
    ${CMAKE_SOURCE_DIR}/Common/
    ${CMAKE_SOURCE_DIR}/Reconstructor/
    ${CMAKE_SOURCE_DIR}/dependencies/
    ${CMAKE_SOURCE_DIR}/Dependencies/imgui/
    ${CMAKE_SOURCE_DIR}/Dependencies/RFGR_Types/
    ${CMAKE_SOURCE_DIR}/Dependencies/asmjit/src/
)

add_compile_definitions(TRACY_IMPORTS)