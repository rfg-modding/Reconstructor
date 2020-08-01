#pragma once
#include "common/patching/FunHook.h"
#include "common/Typedefs.h"
#include "rfg/Player.h"
#include "rsl2/misc/GlobalState.h"
#include "rsl2/functions/Functions.h"
#ifdef COMPILE_IN_PROFILER
#include "tracy/Tracy.hpp"
#endif


//struct gr_state
//{
//    unsigned int value;
//    unsigned int tnl_value;
//};
////bool __cdecl gr_bbox_aligned(vector* mn, vector* mx, gr_state* state) //0015E3A0
//using F_gr_bbox_aligned = bool(__cdecl*)(vector* min, vector* max, gr_state* state);
//extern F_gr_bbox_aligned gr_bbox_aligned;

FunHook<void(Player*)> PlayerDoFrame_hook
{
    0x6E6290, //__cdecl
    [](Player* player)
    {
        if (!GetGlobalState()->Player)
        {
            GetGlobalState()->Player = player;
        }
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

        ////Test out rfg primitive rendering code
        //static gr_state renderState;
        //static bool firstRun = true;
        //if (firstRun)
        //{
        //    rfg::gr_state_constructor(&renderState, nullptr, ALPHA_BLEND_ADDITIVE, CLAMP_MODE_CLAMP, ZBUF_NONE, STENCIL_NONE, 0, CULL_MODE_NONE, TNL_CLIP_MODE_NONE, TNL_LIGHT_MODE_NONE);
        //    firstRun = false;
        //}
        //rfg::gr_bbox_aligned(&player->last_known_bmin, &player->last_known_bmax, &renderState);

        PlayerDoFrame_hook.CallTarget(player);
    }
};