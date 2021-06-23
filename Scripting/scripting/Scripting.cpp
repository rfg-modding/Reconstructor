#include "common/windows/WindowsWrapper.h"
#include "common/plugins/Plugin.h"
#include "rsl2/functions/Functions.h"
#include "rsl2/misc/GlobalState.h"
#include "common/Typedefs.h"
#include "common/Common.h"
#include "common/filesystem/File.h"
#include "common/timing/Timer.h"
#include "rsl2/IRSL2.h"
#include "modules/RfgModule.h"
#include <filesystem>
#include <pybind11/embed.h>
#include <cstdio>
#include <math.h>

IHost* host_ = nullptr;
IRSL2* rsl2_ = nullptr;

bool PythonInitialized = false;
bool PythonThreadStateInitialized = false;
Timer PythonScriptTimer(false);
void PybindInit();
void PybindShutdown();
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

        //Register callbacks
        rsl2_->RegisterPrimitiveDrawCallback(&PrimitiveDrawCallback);

        if(!PythonInitialized)
            PybindInit();

        if (!rsl2_)
        {
            printf("Error! Failed to import all external functions in Scripting.dll::RSL2_PluginInit.\n");
            return false;
        }
        Scripting_GlobalState = rsl2_->GetGlobalState();

        return true;
    }

    //Called when the host dll unloads this plugin
    DLLEXPORT bool __cdecl RSL2_PluginShutdown()
    {
        //Remove callbacks
        rsl2_->RemovePrimitiveDrawCallback(&PrimitiveDrawCallback);
        
        if(PythonInitialized)
            PybindShutdown();

        rsl2_ = nullptr;
        return true;
    }

    //Called immediately before dependency shutdown + unload
    DLLEXPORT void __cdecl RSL2_OnDependencyUnload(const string& dependencyName)
    {
        //Remove callbacks
        rsl2_->RemovePrimitiveDrawCallback(&PrimitiveDrawCallback);

        if(PythonInitialized)
            PybindShutdown();
        
        rsl2_ = nullptr;
    }

    //Called immediately after dependency load + init
    DLLEXPORT void __cdecl RSL2_OnDependencyLoad(const string& dependencyName)
    { 
        //Re-import external functions from dependency
        rsl2_ = (IRSL2*)host_->GetPluginInterface("RSL2", "RSL2");

        if(!PythonInitialized)
            PybindInit();

        //Register callbacks
        rsl2_->RegisterPrimitiveDrawCallback(&PrimitiveDrawCallback);
    }
}

void PybindInit()
{
    printf("----- PybindInit() -----\n");
    pybind11::initialize_interpreter();
    PythonInitialized = true;
}

void PybindShutdown()
{
    printf("----- PybindShutdown() -----\n");
    pybind11::finalize_interpreter();
    PythonInitialized = false;
    PythonThreadStateInitialized = false;
}

std::filesystem::file_time_type PerFrameScriptLastWriteTime;
string PerFrameScriptString;
string PerFrameScriptPath = "G:/GOG/Games/Red Faction Guerrilla Re-Mars-tered/RSL2/Scripts/PerFrameTest0.py";

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

    if (!PythonThreadStateInitialized)
    {
        PyEval_InitThreads();
        PyEval_SaveThread();
        PythonThreadStateInitialized = true;
    }

    //Start python timer
    PythonScriptTimer.Reset();
    
    PyGILState_STATE gstate;
    gstate = PyGILState_Ensure();

    if (std::filesystem::last_write_time(PerFrameScriptPath) != PerFrameScriptLastWriteTime)
    {
        PerFrameScriptString = File::ReadToString(PerFrameScriptPath);
        PerFrameScriptLastWriteTime = std::filesystem::last_write_time(PerFrameScriptPath);
    }
    pybind11::eval<pybind11::eval_statements>(PerFrameScriptString);

    if (globalState->RunPythonTestFile)
    {
        pybind11::eval_file("G:/GOG/Games/Red Faction Guerrilla Re-Mars-tered/RSL2/Scripts/SingleRunTest0.py");
        globalState->RunPythonTestFile = false;
    }

    PyGILState_Release(gstate);

    //End python timer and print time
    f32 elapsedSeconds = (f32)PythonScriptTimer.ElapsedMicroseconds() / 1000000.0f;
    printf("Python script time: %f ms\n", elapsedSeconds * 1000.0f);
}