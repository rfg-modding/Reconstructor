#include "common/windows/WindowsWrapper.h"
#include "common/plugins/Plugin.h"
#include "rsl2/functions/Functions.h"
#include "rsl2/misc/GlobalState.h"
#include "common/timing/Timer.h"
#include "common/Typedefs.h"
#include "common/Common.h"
#include "rsl2/IRSL2.h"
#include <cstdio>
#include "scripting/misc/GlobalState.h"
#define SOL_ALL_SAFETIES 1
#include <sol/sol.hpp>
#include "scripting/modules/RfgModule.h"

bool LuaInitialized = false;
sol::state* Lua = nullptr;
Timer LuaScriptTimer(false);
void LuaInit();
void LuaShutdown();

void PrimitiveDrawCallback();

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
        host_ = host;
        rsl2_ = (IRSL2*)host_->GetPluginInterface("RSL2", "RSL2");

        if (!rsl2_)
        {
            printf("Error! Failed to import all external functions in Scripting.dll::RSL2_PluginInit.\n");
            return false;
        }

        if(!LuaInitialized)
            LuaInit();

        //Register callbacks
        rsl2_->RegisterPrimitiveDrawCallback(&PrimitiveDrawCallback);

        return true;
    }

    //Called when the host dll unloads this plugin
    DLLEXPORT bool __cdecl RSL2_PluginShutdown()
    {
        //Remove callbacks
        rsl2_->RemovePrimitiveDrawCallback(&PrimitiveDrawCallback);

        if(LuaInitialized)
            LuaShutdown();

        rsl2_ = nullptr;
        return true;
    }

    //Called immediately before dependency shutdown + unload
    DLLEXPORT void __cdecl RSL2_OnDependencyUnload(const string& dependencyName)
    {
        //Remove callbacks
        rsl2_->RemovePrimitiveDrawCallback(&PrimitiveDrawCallback);

        if(LuaInitialized)
            LuaShutdown();

        rsl2_ = nullptr;
    }

    //Called immediately after dependency load + init
    DLLEXPORT void __cdecl RSL2_OnDependencyLoad(const string& dependencyName)
    {
        //Re-import external functions from dependency
        rsl2_ = (IRSL2*)host_->GetPluginInterface("RSL2", "RSL2");

        if(!LuaInitialized)
            LuaInit();

        //Register callbacks
        rsl2_->RegisterPrimitiveDrawCallback(&PrimitiveDrawCallback);
    }
}

void LuaInit()
{
    Lua = new sol::state();
    Lua->open_libraries(sol::lib::base, sol::lib::ffi);
    InitRfgLuaModule(*Lua);
    LuaInitialized = true;
}

void LuaShutdown()
{
    delete Lua;
    LuaInitialized = false;
}

gr_state renderState;
void PrimitiveDrawCallback()
{
    //Wait until ImGui hooks are initialized. It's a decent sign that all renderer hooks are in place and ready for use
    RSL2_GlobalState* globalState = rsl2_->GetGlobalState();
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

    //Start script timer
    LuaScriptTimer.Reset();

    auto result = Lua->safe_script_file("G:/GOG/Games/Red Faction Guerrilla Re-Mars-tered/RSL2/Scripts/PerFrameTestLua0.lua");
    if (!result.valid())
    {
        sol::error error = result;
        printf("Lua script error: %s\n", error.what());
    }

    //End script timer and print time
    f32 elapsedSeconds = (f32)LuaScriptTimer.ElapsedMicroseconds() / 1000000.0f;
    //printf("Per frame lua script time: %f ms\n", elapsedSeconds * 1000.0f);
}