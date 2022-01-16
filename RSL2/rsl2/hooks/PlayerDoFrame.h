#pragma once
#include "common/patching/FunHook.h"
#include "common/Typedefs.h"
#include "rfg/Player.h"
#include "rsl2/misc/GlobalState.h"
#include "rsl2/functions/Functions.h"
#include "rsl2/hooks/Camera.h"
#ifdef COMPILE_IN_PROFILER
#include "tracy/Tracy.hpp"
#endif

FunHook<void(player*)> PlayerDoFrame_hook
{
    0x6E6290,
    [](player* player)
    {
#ifdef COMPILE_IN_PROFILER
        ZoneScopedN("PlayerDoFrame");
#endif
        if (GetGlobalState()->Player != player)
            GetGlobalState()->Player = player;

        //Todo: Provide a per-frame callback that stuff like CameraUpdate can be ran via. Sticking everything in this hook isn't sustainable
        //Update camera
        CameraUpdate();

        PlayerDoFrame_hook.CallTarget(player);
    }
};