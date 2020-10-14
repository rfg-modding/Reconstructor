#include "common/windows/WindowsWrapper.h"
#include "guiBase/ImportedFunctions.h"
#include "common/plugins/Plugin.h"
#include "rsl2/functions/Functions.h"
#include "rsl2/misc/GlobalState.h"
#include "common/Typedefs.h"
#include "common/Common.h"
#include "gui/DebugGui.h"
#include "debugDraw/debugDraw.h"
#include "imgui.h"
#include <cstdio>

IHost* host_ = nullptr;
void ImGuiCallback();
void OverlayCallback();
void PrimitiveDrawCallback();

bool ImGuiContextInitialized = false;

extern "C"
{
    //Called before init
    DLLEXPORT PluginInfo __cdecl RSL2_PluginInfo()
    {
        return PluginInfo
        {
            .Dependencies = { "RSL2" }
        };
    }

    //Called when the host dll loads this plugin
    DLLEXPORT bool __cdecl RSL2_PluginInit(IHost* host, std::vector<PluginFunction>& exportedFunctions)
    {
        printf("GuiBase.dll RSL2_PluginInit() called!!\n");
        host_ = host;

        //External functions used by this plugin
        ext_GetGlobalState = (F_ext_GetGlobalState)host_->GetPluginFunction("RSL2", "GetGlobalState");
        ext_GetRfgFunctions = (F_ext_GetRfgFunctions)host_->GetPluginFunction("RSL2", "GetRfgFunctions");
        ext_GetImGuiContext = (F_ext_GetImGuiContext)host_->GetPluginFunction("RSL2", "GetImGuiContext");
        ext_RegisterImGuiCallback = (F_ext_RegisterImGuiCallback)host_->GetPluginFunction("RSL2", "RegisterImGuiCallback");
        ext_RemoveImGuiCallback = (F_ext_RemoveImGuiCallback)host_->GetPluginFunction("RSL2", "RemoveImGuiCallback");
        ext_RegisterOverlayCallback = (F_ext_RegisterOverlayCallback)host_->GetPluginFunction("RSL2", "RegisterOverlayCallback");
        ext_RemoveOverlayCallback = (F_ext_RemoveOverlayCallback)host_->GetPluginFunction("RSL2", "RemoveOverlayCallback");
        ext_RegisterPrimitiveDrawCallback = (F_ext_RegisterPrimitiveDrawCallback)host_->GetPluginFunction("RSL2", "RegisterPrimitiveDrawCallback");
        ext_RemovePrimitiveDrawCallback = (F_ext_RemovePrimitiveDrawCallback)host_->GetPluginFunction("RSL2", "RemovePrimitiveDrawCallback");

        if (!ext_GetGlobalState ||
            !ext_GetRfgFunctions ||
            !ext_GetImGuiContext ||
            !ext_RegisterImGuiCallback ||
            !ext_RemoveImGuiCallback ||
            !ext_RegisterOverlayCallback ||
            !ext_RemoveOverlayCallback ||
            !ext_RegisterPrimitiveDrawCallback ||
            !ext_RemovePrimitiveDrawCallback)
        {
            printf("Error! Failed to import all external functions in GuiBase.dll::RSL2_PluginInit.\n");
            return false;
        }

        //Register callbacks
        ext_RegisterImGuiCallback(&ImGuiCallback);
        ext_RegisterOverlayCallback(&OverlayCallback);
        ext_RegisterPrimitiveDrawCallback(&PrimitiveDrawCallback);

        return true;
    }

    //Called when the host dll unloads this plugin
    DLLEXPORT bool __cdecl RSL2_PluginShutdown()
    {
        printf("GuiBase.dll RSL2_PluginShutdown() called!\n");

        //Remove callbacks
        ext_RemoveImGuiCallback(&ImGuiCallback);
        ext_RemoveOverlayCallback(&OverlayCallback);
        ext_RemovePrimitiveDrawCallback(&PrimitiveDrawCallback);

        ext_GetGlobalState = nullptr;
        ext_GetRfgFunctions = nullptr;
        ext_GetImGuiContext = nullptr;
        ext_RegisterImGuiCallback = nullptr;
        ext_RemoveImGuiCallback = nullptr;
        ext_RegisterOverlayCallback = nullptr;
        ext_RemoveOverlayCallback = nullptr;
        ext_RegisterPrimitiveDrawCallback = nullptr;
        ext_RemovePrimitiveDrawCallback = nullptr;

        ImGuiContextInitialized = false;

        return true;
    }

    //Called immediately before dependency shutdown + unload
    DLLEXPORT void __cdecl RSL2_OnDependencyUnload(const string& dependencyName)
    {
        //Remove callbacks
        ext_RemoveImGuiCallback(&ImGuiCallback);
        ext_RemoveOverlayCallback(&OverlayCallback);
        ext_RemovePrimitiveDrawCallback(&PrimitiveDrawCallback);

        ext_GetGlobalState = nullptr;
        ext_GetRfgFunctions = nullptr;
        ext_GetImGuiContext = nullptr;
        ext_RegisterImGuiCallback = nullptr;
        ext_RemoveImGuiCallback = nullptr;
        ext_RegisterOverlayCallback = nullptr;
        ext_RemoveOverlayCallback = nullptr;
        ext_RegisterPrimitiveDrawCallback = nullptr;
        ext_RemovePrimitiveDrawCallback = nullptr;

        ImGuiContextInitialized = false;
    }

    //Called immediately after dependency load + init
    DLLEXPORT void __cdecl RSL2_OnDependencyLoad(const string& dependencyName)
    { 
        //Re-import external functions from dependency
        ext_GetGlobalState = (F_ext_GetGlobalState)host_->GetPluginFunction("RSL2", "GetGlobalState");
        ext_GetRfgFunctions = (F_ext_GetRfgFunctions)host_->GetPluginFunction("RSL2", "GetRfgFunctions");
        ext_GetImGuiContext = (F_ext_GetImGuiContext)host_->GetPluginFunction("RSL2", "GetImGuiContext");
        ext_RegisterImGuiCallback = (F_ext_RegisterImGuiCallback)host_->GetPluginFunction("RSL2", "RegisterImGuiCallback");
        ext_RemoveImGuiCallback = (F_ext_RemoveImGuiCallback)host_->GetPluginFunction("RSL2", "RemoveImGuiCallback");
        ext_RegisterOverlayCallback = (F_ext_RegisterOverlayCallback)host_->GetPluginFunction("RSL2", "RegisterOverlayCallback");
        ext_RemoveOverlayCallback = (F_ext_RemoveOverlayCallback)host_->GetPluginFunction("RSL2", "RemoveOverlayCallback");
        ext_RegisterPrimitiveDrawCallback = (F_ext_RegisterPrimitiveDrawCallback)host_->GetPluginFunction("RSL2", "RegisterPrimitiveDrawCallback");
        ext_RemovePrimitiveDrawCallback = (F_ext_RemovePrimitiveDrawCallback)host_->GetPluginFunction("RSL2", "RemovePrimitiveDrawCallback");

        //Register callbacks
        ext_RegisterImGuiCallback(&ImGuiCallback);
        ext_RegisterOverlayCallback(&OverlayCallback);
        ext_RegisterPrimitiveDrawCallback(&PrimitiveDrawCallback);
    }
}

void ImGuiCallback()
{
    RSL2_GlobalState* globalState = ext_GetGlobalState();
    if (!ImGuiContextInitialized)
    {
        if (globalState->ImGuiInitialized)
        {
            ImGui::SetCurrentContext(ext_GetImGuiContext());
            ImGuiContextInitialized = true;
        }
        else
        {
            return;
        }
    }

    ImGui::ShowDemoWindow();
    DebugGui_DoFrame();
}

void OverlayCallback()
{
    RSL2_GlobalState* globalState = ext_GetGlobalState();
    if (!ImGuiContextInitialized)
        return; //Don't bother repeating the imgui init code here since GUI callbacks are ran first

    DebugOverlay_DoFrame();
}

void PrimitiveDrawCallback()
{
    RSL2_GlobalState* globalState = ext_GetGlobalState();
    //Wait until ImGui hooks are initialized. It's a decent sign that all renderer hooks are in place and ready for use
    if (!globalState->ImGuiInitialized)
        return; 

    //Initialize default render state used by all primitive draw funcs
    rfg::RfgFunctions* Functions = ext_GetRfgFunctions();
    static bool firstRun = true;
    if (firstRun)
    {
        Functions->gr_state_constructor(&renderState, nullptr, ALPHA_BLEND_ADDITIVE, CLAMP_MODE_CLAMP, ZBUF_NONE, STENCIL_NONE, 0, CULL_MODE_CULL, TNL_CLIP_MODE_NONE, TNL_LIGHT_MODE_NONE);
        firstRun = false;
    }

    GeneralDebugDraw_DoFrame();
    RfgOldMemoryOverlay_DoFrame();
}