#include "rsl2/functions/FunctionsInternal.h"
#include "common/string/String.h"
#include <filesystem>
#ifdef COMPILE_IN_PROFILER
#include "tracy/Tracy.hpp"
#endif

//Note: For debugging purposes. Disabled by default.

//void __cdecl keen::debug::nativePrint(const char *pMessage) //0x007F4570
FunHook<void __cdecl(const char* pMessage)> keen_debug_nativePrint_hook
{
    0x007F4570,
    [](const char* pMessage) -> void
    {
        keen_debug_nativePrint_hook.CallTarget(pMessage);
    }
};