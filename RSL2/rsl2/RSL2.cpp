#include "common/Typedefs.h"
#include "common/windows/WindowsWrapper.h"
#include "common/patching/FunHook.h"
#include "common/Common.h"
#include <cstdio>

//Todo: Have common lib with RFG types instead of tossing them everywhere
#include "rfg/Player.h"

//Note: When a plugin is reloaded the host called RSL2_PluginUnload and then RSL2_PluginInit agai
//      as if it were the first time the host were loading the plugin.

static uintptr_t ModuleBase = 0;

FunHook<void(Player*)> PlayerDoFrame_hook
{
    0x6E6290, //__cdecl
    [](Player* player)
    {
        printf("In PlayerDoFrame() hook\n");
        Object* playerObj = reinterpret_cast<Object*>(player);
        static unsigned int counter = 0;
        if (counter % 20 == 0)
        {
            printf("Player position: %s\n", playerObj->pos.GetDataString(true, true).c_str());
        }
        counter++;
        PlayerDoFrame_hook.CallTarget(player);
    },
};


//Need to use extern "C" to avoid C++ export name mangling. Lets us use the exact name RSL2_XXXX with GetProcAddress in the host
extern "C"
{
    //Called when the host dll loads this plugin
    DLLEXPORT bool __cdecl RSL2_PluginInit()
    {
        printf("RSL2.dll RSL2_PluginInit() called!\n");

        ModuleBase = reinterpret_cast<uintptr_t>(GetModuleHandle(nullptr));
        printf("ModuleBase: %d\n", ModuleBase);
        PlayerDoFrame_hook.SetAddr(ModuleBase + 0x6E6290);
        PlayerDoFrame_hook.Install();

        return true;
    }

    //Called when the host dll unloads this plugin
    DLLEXPORT bool __cdecl RSL2_PluginUnload()
    {
        printf("RSL2.dll RSL2_PluginUnload() called!\n");
        
        PlayerDoFrame_hook.Remove();

        return true;
    }
}