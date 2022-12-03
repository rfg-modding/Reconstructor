#include "common/windows/WindowsWrapper.h"
#include "common/plugins/Plugin.h"
#include "rsl2/functions/Functions.h"
#include "rsl2/misc/GlobalState.h"
#include "common/Typedefs.h"
#include "common/patching/Config.h"
#include "common/Common.h"
#include "devTools/misc/GlobalState.h"
#include "rsl2/IRSL2.h"
#include <cstdio>

void ImGuiCallback();

bool ImGuiContextInitialized = false;

extern "C"
{
    //Called before init
    DLLEXPORT PluginInfo __cdecl RSL2_PluginInfo()
    {
        return PluginInfo
        {
            .Dependencies =
            {
                "RSL2"
            }
        };
    }

    //Called when the host dll loads this plugin
    DLLEXPORT bool __cdecl RSL2_PluginInit(IHost* host, std::vector<PluginInterface>& exportedFunctions)
    {
        host_ = host;

        //External functions used by this plugin
        rsl2_ = (IRSL2*)host_->GetPluginInterface("RSL2", "RSL2");
        if (!rsl2_)
        {
            printf("Error! Failed to import all external functions in DevTools.dll::RSL2_PluginInit.\n");
            return false;
        }

        RSL2_GlobalState* globalState = rsl2_->GetGlobalState();
        if (!globalState)
        {
            printf("Error! Failed to get RSL2 global state in DevTools.dll::RSL2_PluginInit.\n");
            return false;
        }

        //Initialize common lib
        CommonLib_ModuleBase = globalState->ModuleBase;

        return true;
    }

    //Called when the host dll unloads this plugin
    DLLEXPORT bool __cdecl RSL2_PluginShutdown()
    {
        rsl2_ = nullptr;
        ImGuiContextInitialized = false;

        return true;
    }

    //Called immediately before dependency shutdown + unload
    DLLEXPORT void __cdecl RSL2_OnDependencyUnload(const string& dependencyName)
    {
        rsl2_ = nullptr;
        ImGuiContextInitialized = false;
    }

    //Called immediately after dependency load + init
    DLLEXPORT void __cdecl RSL2_OnDependencyLoad(const string& dependencyName)
    {
        //Re-import external functions from dependency
        rsl2_ = (IRSL2*)host_->GetPluginInterface("RSL2", "RSL2");
    }
}