#pragma once
#include "common/patching/FunHook.h"
#include "common/Typedefs.h"
#include "rfg/Player.h"
#include "reconstructor/misc/GlobalState.h"
#include "reconstructor/util/Util.h"
#include "reconstructor/functions/Functions.h"
#include "reconstructor/hooks/Camera.h"
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

//void __cdecl rfg_init_stage_2_done(keen::SoundSystem::System *pSoundSystem) //0x001D55C0
FunHook<void(void* pSoundSystem)> rfg_init_stage_2_done_hook
{
    0x001D55C0,
    [](void* pSoundSystem)
    {
        //Call init function
        rfg_init_stage_2_done_hook.CallTarget(pSoundSystem);

        //Ensure global state vars are initialized. Some rely on values that created by the game in rfg_init_stage_2_done()
        InitGlobals();
    }
};

// void __cdecl gameseq_push_state(game_state new_state, bool transparent, bool pause_beneath) //0x003D87E0
FunHook<void __cdecl(game_state new_state, bool transparent, bool pause_beneath)> gameseqPushState_hook
{
    0x003D87E0,
    [](game_state new_state, bool transparent, bool pause_beneath) -> void
    {
        if (new_state == GS_MULTIPLAYER_SEARCH_MATCHMAKING || new_state == GS_MULTIPLAYER_CHANGE_MATCHMAKING) {
            // do nothing
        } else {
            gameseqPushState_hook.CallTarget(new_state, transparent, pause_beneath);
        }
    }
};
