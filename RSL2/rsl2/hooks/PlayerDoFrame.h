#pragma once
#include "common/patching/FunHook.h"
#include "common/Typedefs.h"
#include "rfg/Player.h"
#ifdef COMPILE_IN_PROFILER
#include "tracy/Tracy.hpp"
#endif

FunHook<void(Player*)> PlayerDoFrame_hook
{
    0x6E6290, //__cdecl
    [](Player* player)
    {
        //Todo: Make RSL work with profiler without failing to load the DLL
        //Todo: Wrap tracy macros in our own that can work even when it's not compiled in
#ifdef COMPILE_IN_PROFILER
        ZoneScopedN("PlayerDoFrame");
#endif
        Object* playerObj = reinterpret_cast<Object*>(player);
        static u32 counter = 0;

        if (counter % 240 == 0)
        {
            printf("Player position: %s\n", playerObj->pos.GetDataString(true, true).c_str());
        }
        counter++;
        PlayerDoFrame_hook.CallTarget(player);
    }
};