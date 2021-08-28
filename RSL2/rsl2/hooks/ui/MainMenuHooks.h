#pragma once
#include "common/patching/FunHook.h"
#include "common/Typedefs.h"
#include "rsl2/misc/GlobalState.h"
#include "rsl2/functions/Functions.h"
#include "rsl2/util/Util.h"
#include "common/BuildConfig.h"
#ifdef COMPILE_IN_PROFILER
#include "tracy/Tracy.hpp"
#endif

//void __cdecl main_menu_process() //0x00513770
FunHook<void ()> main_menu_process_hook
{
    0x00513770,
    []()
    {
        static u32 mainMenuFrames = 0; //Number of frames the main menu has been alive
        if (mainMenuFrames > 100) //Wait 100 frames to ensure the main menu has been created by the game
        {
            //Hide invalid RSL main menu options (MP and WC). Done in a hook to ensure the main menu exists first.
            //Previously was done in RSL2_PluginInit and it could sometimes fail if it was done too early.
            TryHideInvalidMainMenuOptions();
        }
        else
        {
            mainMenuFrames++;
        }

        main_menu_process_hook.CallTarget();
    }
};