cmake_minimum_required (VERSION 3.8)
project (Scripting VERSION 1.0.0
                  DESCRIPTION "Scripting"
                  LANGUAGES CXX)

# Make sure cmake doesn't add the build config name to the end of built DLLs (eg. Scripting_debug.dll instead of Scripting.dll)
set(CMAKE_DEBUG_POSTFIX "")
set(CMAKE_RELEASE_POSTFIX "")
set(CMAKE_RELWITHDEBINFO_POSTFIX "")

# Get list of source files in project src folders recursively
# Places in SOURCES variable
file(GLOB_RECURSE SOURCES
    ${CMAKE_SOURCE_DIR}/Scripting/scripting/*.cpp
    ${CMAKE_SOURCE_DIR}/Scripting/scripting/*.h
)

link_directories(${CMAKE_SOURCE_DIR}/dependencies/RSL2-Dependencies/LuaJIT/src/)
add_compile_definitions(IMGUI_IMPL_WIN32_DISABLE_LINKING_XINPUT IMGUI_IMPL_WIN32_DISABLE_GAMEPAD)

# Add source to this project's DLL
add_library(Scripting SHARED ${SOURCES}
            ${CMAKE_SOURCE_DIR}/dependencies/imgui/imgui.cpp
            ${CMAKE_SOURCE_DIR}/dependencies/imgui/imgui_demo.cpp
            ${CMAKE_SOURCE_DIR}/dependencies/imgui/imgui_draw.cpp
            ${CMAKE_SOURCE_DIR}/dependencies/imgui/imgui_widgets.cpp
            ${CMAKE_SOURCE_DIR}/dependencies/imgui/imgui_tables.cpp
            ${CMAKE_SOURCE_DIR}/dependencies/imgui/misc/cpp/imgui_stdlib.cpp
            ${CMAKE_SOURCE_DIR}/dependencies/imgui/backends/imgui_impl_win32.cpp
            ${CMAKE_SOURCE_DIR}/dependencies/imgui/backends/imgui_impl_dx11.cpp
            )

target_link_libraries(Scripting PRIVATE RFGR_Types)
target_link_libraries(Scripting PRIVATE Common)
target_link_libraries(Scripting PRIVATE lua51.lib)
target_link_libraries(Scripting PRIVATE luajit.lib)
target_link_libraries(Scripting PRIVATE sol2)

# Link profiler DLL if it's flag is set
if(COMPILE_IN_PROFILER)
    target_link_libraries(Scripting PRIVATE Profiler)
endif()

# Add include directories
target_include_directories(Scripting SYSTEM PRIVATE
    ${CMAKE_SOURCE_DIR}/Common/
    ${CMAKE_SOURCE_DIR}/Scripting/
    ${CMAKE_SOURCE_DIR}/RSL2/
    ${CMAKE_SOURCE_DIR}/dependencies/
    ${CMAKE_SOURCE_DIR}/dependencies/imgui/
    ${CMAKE_SOURCE_DIR}/dependencies/RFGR_Types/
    ${CMAKE_SOURCE_DIR}/dependencies/
    ${CMAKE_SOURCE_DIR}/dependencies/RSL2-Dependencies/LuaJIT/src/
)

add_compile_definitions(TRACY_IMPORTS)