#include "common/windows/WindowsWrapper.h"

//This DLL is included by other DLLs which want to use tracy profiler. Necessary for multi-DLL projects like this one
//Doesn't need to do anything, including tracy in the CMakeLists with the TRACY_EXPORTS define exports all the necessary functions
BOOL WINAPI DllMain(HINSTANCE handle, DWORD reason, LPVOID reserved)
{
    return TRUE;
}