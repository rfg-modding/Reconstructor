#include "common/windows/WindowsWrapper.h"
#include "common/plugins/Plugin.h"
#include "reconstructor/functions/Functions.h"
#include "reconstructor/misc/GlobalState.h"
#include "common/Typedefs.h"
#include "common/patching/Config.h"
#include "common/Common.h"
#include "devTools/misc/GlobalState.h"
#include "reconstructor/IReconstructor.h"
#include "gui/MiscToolsGui.h"
#include "DevToolsWndProc.h"
#include <cstdio>

MiscToolsGui MiscTools;

extern "C"
{
    //Called before init
    DLLEXPORT PluginInfo __cdecl Reconstructor_PluginInfo()
    {
        return PluginInfo
        {
            .Dependencies =
            {
                "Reconstructor"
            }
        };
    }

    //Called when the host dll loads this plugin
    DLLEXPORT bool __cdecl Reconstructor_PluginInit(IHost* host, std::vector<PluginInterface>& exportedFunctions)
    {
        host_ = host;

        //External functions used by this plugin
        reconstructor = (IReconstructor*)host_->GetPluginInterface("Reconstructor", "Reconstructor");
        if (!reconstructor)
        {
            printf("Error! Failed to import all external functions in DevTools.dll::Reconstructor_PluginInit.\n");
            return false;
        }

        Reconstructor_GlobalState* globalState = reconstructor->GetGlobalState();
        if (!globalState)
        {
            printf("Error! Failed to get Reconstructor global state in DevTools.dll::Reconstructor_PluginInit.\n");
            return false;
        }

        //Initialize common lib
        CommonLib_ModuleBase = globalState->ModuleBase;

        MiscTools.Open = true; //Open by default since it's the only debug gui currently
        reconstructor->RegisterGui(&MiscTools);
        reconstructor->AddWndProcCallback(&DevToolsWndProc);
        return true;
    }

    //Called when the host dll unloads this plugin
    DLLEXPORT bool __cdecl Reconstructor_PluginShutdown()
    {
        reconstructor->RemoveGui(&MiscTools);
        reconstructor = nullptr;
        ImGuiContextInitialized = false;

        return true;
    }

    //Called immediately before dependency shutdown + unload
    DLLEXPORT void __cdecl Reconstructor_OnDependencyUnload(const string& dependencyName)
    {
        reconstructor->RemoveGui(&MiscTools);
        reconstructor = nullptr;
        ImGuiContextInitialized = false;
    }

    //Called immediately after dependency load + init
    DLLEXPORT void __cdecl Reconstructor_OnDependencyLoad(const string& dependencyName)
    {
        //Re-import external functions from dependency
        reconstructor = (IReconstructor*)host_->GetPluginInterface("Reconstructor", "Reconstructor");
        reconstructor->RegisterGui(&MiscTools);
        reconstructor->AddWndProcCallback(&DevToolsWndProc);
    }
}