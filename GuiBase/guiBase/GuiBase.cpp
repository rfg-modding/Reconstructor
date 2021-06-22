#include "common/windows/WindowsWrapper.h"
#include "common/plugins/Plugin.h"
#include "rsl2/functions/Functions.h"
#include "rsl2/misc/GlobalState.h"
#include "common/Typedefs.h"
#include "common/Common.h"
#include "gui/DebugGui.h"
#include "debugDraw/debugDraw.h"
#include <imgui.h>
#include "rsl2/IRSL2.h"
#include <cstdio>

IHost* host_ = nullptr;
IRSL2* rsl2_ = nullptr;
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
    DLLEXPORT bool __cdecl RSL2_PluginInit(IHost* host, std::vector<PluginInterface>& exportedFunctions)
    {
        printf("GuiBase.dll RSL2_PluginInit() called!!\n");
        host_ = host;

        //External functions used by this plugin
        rsl2_ = (IRSL2*)host_->GetPluginInterface("RSL2", "RSL2");

        if (!rsl2_)
        {
            printf("Error! Failed to import all external functions in GuiBase.dll::RSL2_PluginInit.\n");
            return false;
        }

        //Register callbacks
        rsl2_->RegisterImGuiCallback(&ImGuiCallback);
        rsl2_->RegisterOverlayCallback(&OverlayCallback);
        rsl2_->RegisterPrimitiveDrawCallback(&PrimitiveDrawCallback);

        return true;
    }

    //Called when the host dll unloads this plugin
    DLLEXPORT bool __cdecl RSL2_PluginShutdown()
    {
        printf("GuiBase.dll RSL2_PluginShutdown() called!\n");

        //Remove callbacks
        rsl2_->RemoveImGuiCallback(&ImGuiCallback);
        rsl2_->RemoveOverlayCallback(&OverlayCallback);
        rsl2_->RemovePrimitiveDrawCallback(&PrimitiveDrawCallback);

        rsl2_ = nullptr;
        ImGuiContextInitialized = false;

        return true;
    }

    //Called immediately before dependency shutdown + unload
    DLLEXPORT void __cdecl RSL2_OnDependencyUnload(const string& dependencyName)
    {
        //Remove callbacks
        rsl2_->RemoveImGuiCallback(&ImGuiCallback);
        rsl2_->RemoveOverlayCallback(&OverlayCallback);
        rsl2_->RemovePrimitiveDrawCallback(&PrimitiveDrawCallback);

        rsl2_ = nullptr;
        ImGuiContextInitialized = false;
    }

    //Called immediately after dependency load + init
    DLLEXPORT void __cdecl RSL2_OnDependencyLoad(const string& dependencyName)
    { 
        //Re-import external functions from dependency
        rsl2_ = (IRSL2*)host_->GetPluginInterface("RSL2", "RSL2");

        //Register callbacks
        rsl2_->RegisterImGuiCallback(&ImGuiCallback);
        rsl2_->RegisterOverlayCallback(&OverlayCallback);
        rsl2_->RegisterPrimitiveDrawCallback(&PrimitiveDrawCallback);
    }
}

void ImGuiCallback()
{
    RSL2_GlobalState* globalState = rsl2_->GetGlobalState();
    if (!ImGuiContextInitialized)
    {
        if (globalState->ImGuiInitialized)
        {
            ImGui::SetCurrentContext(rsl2_->GetImGuiContext());
            ImGuiContextInitialized = true;
        }
        else
        {
            return;
        }
    }

    ImGui::ShowDemoWindow();
    DebugGui_DoFrame(rsl2_);
}

void OverlayCallback()
{
    RSL2_GlobalState* globalState = rsl2_->GetGlobalState();
    if (!ImGuiContextInitialized)
        return; //Don't bother repeating the imgui init code here since GUI callbacks are ran first

    DebugOverlay_DoFrame(rsl2_);
}

void PrimitiveDrawCallback()
{
    RSL2_GlobalState* globalState = rsl2_->GetGlobalState();
    //Wait until ImGui hooks are initialized. It's a decent sign that all renderer hooks are in place and ready for use
    if (!globalState->ImGuiInitialized)
        return; 

    //Initialize default render state used by all primitive draw funcs
    rfg::RfgFunctions* Functions = rsl2_->GetRfgFunctions();
    static bool firstRun = true;
    if (firstRun)
    {
        Functions->gr_state_constructor(&renderState, nullptr, ALPHA_BLEND_ADDITIVE, CLAMP_MODE_CLAMP, ZBUF_NONE, STENCIL_NONE, 0, CULL_MODE_CULL, TNL_CLIP_MODE_NONE, TNL_LIGHT_MODE_NONE);
        firstRun = false;
    }

    GeneralDebugDraw_DoFrame(rsl2_);
    RfgOldMemoryOverlay_DoFrame(rsl2_);
}