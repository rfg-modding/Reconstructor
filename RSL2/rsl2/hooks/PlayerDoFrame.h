#pragma once
#include "common/patching/FunHook.h"
#include "common/Typedefs.h"
#include "rfg/Player.h"

FunHook<void(Player*)> PlayerDoFrame_hook
{
    0x6E6290, //__cdecl
    [](Player* player)
    {
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