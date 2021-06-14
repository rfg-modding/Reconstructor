#pragma once
#include "common/patching/FunHook.h"
#include "common/Typedefs.h"
#include "rsl2/misc/GlobalState.h"
#include "rsl2/functions/Functions.h"
#include <RFGR_Types/rfg/stream2/cfile.h>
#include "common/BuildConfig.h"
#ifdef COMPILE_IN_PROFILER
#include "tracy/Tracy.hpp"
#endif

//bool __cdecl cf_add_search(cf_search_types search_type, cf_io_media_types media_type, bool use_first) //0x00197A80
FunHook<bool (cf_search_types search_type, cf_io_media_types media_type, bool use_first)> cf_add_search_hook
{
    0x00197A80,
    [](cf_search_types search_type, cf_io_media_types media_type, bool use_first) -> bool
    {
        printf("cf_add_search_hook():: \n");
        printf("    search_type: %s\n", to_string(search_type).c_str());
        printf("    media_type: %s\n", to_string(media_type).c_str());
        printf("    use_first: %s\n", use_first ? "true" : "false");

        bool result0 = cf_add_search_hook.CallTarget(search_type, media_type, use_first);
        bool result1 = cf_add_search_hook.CallTarget(cf_search_types::CF_SEARCH_STANDARD, cf_io_media_types::CF_IO_MEDIA_HDD, true);
        return result0 && result1;
    }
};