cmake_minimum_required (VERSION 3.8)
project (DevTools VERSION 1.0.0
                  DESCRIPTION "DevTools"
                  LANGUAGES CXX)

# Make sure cmake doesn't add the build config name to the end of built DLLs (eg. DevTools_debug.dll instead of DevTools.dll)
set(CMAKE_DEBUG_POSTFIX "")
set(CMAKE_RELEASE_POSTFIX "")
set(CMAKE_RELWITHDEBINFO_POSTFIX "")

# Get list of source files in project src folders recursively
# Places in SOURCES variable
file(GLOB_RECURSE SOURCES
    ${CMAKE_SOURCE_DIR}/DevTools/devTools/*.cpp
    ${CMAKE_SOURCE_DIR}/DevTools/devTools/*.h
)

add_compile_definitions(IMGUI_IMPL_WIN32_DISABLE_LINKING_XINPUT IMGUI_IMPL_WIN32_DISABLE_GAMEPAD)

# Add source to this project's DLL
add_library(DevTools SHARED ${SOURCES}
            ${CMAKE_SOURCE_DIR}/dependencies/imgui/imgui.cpp
            ${CMAKE_SOURCE_DIR}/dependencies/imgui/imgui_demo.cpp
            ${CMAKE_SOURCE_DIR}/dependencies/imgui/imgui_draw.cpp
            ${CMAKE_SOURCE_DIR}/dependencies/imgui/imgui_widgets.cpp
            ${CMAKE_SOURCE_DIR}/dependencies/imgui/imgui_tables.cpp
            ${CMAKE_SOURCE_DIR}/dependencies/imgui/misc/cpp/imgui_stdlib.cpp
            ${CMAKE_SOURCE_DIR}/dependencies/imgui/backends/imgui_impl_win32.cpp
            ${CMAKE_SOURCE_DIR}/dependencies/imgui/backends/imgui_impl_dx11.cpp
            )

target_link_libraries(DevTools PRIVATE RFGR_Types)
target_link_libraries(DevTools PRIVATE Common)

# Compile in profiler if it's flag is set
if(COMPILE_IN_PROFILER)
    target_link_libraries(DevTools PRIVATE Profiler)
endif()

# Add include directories
target_include_directories(DevTools SYSTEM PRIVATE
    ${CMAKE_SOURCE_DIR}/Common/
    ${CMAKE_SOURCE_DIR}/DevTools/
    ${CMAKE_SOURCE_DIR}/LuaScripting/
    ${CMAKE_SOURCE_DIR}/RSL2/
    ${CMAKE_SOURCE_DIR}/dependencies/
    ${CMAKE_SOURCE_DIR}/dependencies/imgui/
    ${CMAKE_SOURCE_DIR}/dependencies/RFGR_Types/
    ${CMAKE_SOURCE_DIR}/dependencies/
)

add_compile_definitions(TRACY_IMPORTS)