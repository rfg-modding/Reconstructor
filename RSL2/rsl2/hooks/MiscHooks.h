#pragma once
#include "common/patching/FunHook.h"
#include "common/Typedefs.h"
#include "rfg/Player.h"
#include "rsl2/misc/GlobalState.h"
#include "rsl2/functions/Functions.h"
#include "rsl2/hooks/Camera.h"
#include "common/BuildConfig.h"
#ifdef COMPILE_IN_PROFILER
#include "tracy/Tracy.hpp"
#endif

//const char *__cdecl keen::getBuildVersionString() //0x00058740
FunHook<const char* ()> keen_getBuildVersionString_hook
{
    0x00058740,
    []() -> const char*
    {
        return config::BuildVersion;
    }
};