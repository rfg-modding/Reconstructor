#include "common/windows/WindowsWrapper.h"
#include "common/patching/FunHook.h"
#include "common/plugins/Plugin.h"
#include "common/plugins/IHost.h"
#include "hooks/PlayerDoFrame.h"
#include "hooks/MiscHooks.h"
#include "hooks/XmlHooks.h"
#include "hooks/ErrorHooks.h"
#include "hooks/ui/MainMenuHooks.h"
#include "functions/FunctionsInternal.h"
#include "hooks/GrdRenderHooks.h"
#include "hooks/RenderHooks.h"
#include "hooks/WndProc.h"
#include "hooks/MemoryHooks.h"
#include "misc/GlobalState.h"
#include "common/Typedefs.h"
#include "common/Common.h"
#include "common/patching/Config.h"
#include "common/patching/Offset.h"
#include "hooks/MpHooks.h"
#include "hooks/AnimationHooks.h"
#include "gui/OverlayGuis.h"
#include "reconstructor/hooks/Camera.h"
#include "patching/Patches.h"
#include "reconstructor/util/Util.h"
#include "reconstructor/IReconstructor.h"
#include "reconstructor/gui/OverlayGuis.h"
#ifdef COMPILE_IN_PROFILER
#include "tracy/Tracy.hpp"
#endif
#include <cstdio>

//Note: When a plugin is reloaded the host calls Reconstructor_PluginUnload and then Reconstructor_PluginInit again
//      as if it were the first time the host were loading the plugin.

IReconstructor ExportInterface;
void FillExports();

//Need to use extern "C" to avoid C++ export name mangling. Lets us use the exact name Reconstructor_XXXX with GetProcAddress in the host
extern "C"
{
    //Called when a plugin this one depends on is loaded/unloaded. This one has no dependencies so they'll never be called
    DLLEXPORT void __cdecl Reconstructor_OnDependencyUnload(const string& dependencyName) { } //Called immediately before dependency shutdown + unload
    DLLEXPORT void __cdecl Reconstructor_OnDependencyLoad(const string& dependencyName) { } //Called immediately after dependency load + init

    //Called before init. Returns info like a dependencies list used to determine plugin load order
    DLLEXPORT PluginInfo __cdecl Reconstructor_PluginInfo()
    {
        return PluginInfo
        {
            .Dependencies = {}
        };
    }

    //Called when the host dll loads this plugin
    DLLEXPORT bool __cdecl Reconstructor_PluginInit(IHost* host, std::vector<PluginInterface>& exportedFunctions)
    {
        Reconstructor_GlobalState* globalState = GetGlobalState();
        globalState->Host = host;
        globalState->ModuleBase = reinterpret_cast<uintptr_t>(GetModuleHandle(nullptr));
        CommonLib_ModuleBase = globalState->ModuleBase;
        printf("ModuleBase: %d\n", globalState->ModuleBase);

        InitGlobals();
        rfg::Functions = {}; //Initialize pointers to RFG functions
        InitCameraPatches();

        //Rendering hooks
        keen_graphics_beginFrame.Install();
        keen_graphics_endFrame.Install();
        grd_string_hook.Install();
        grd_3d_string_hook.Install();
        grd_get_string_size_hook.Install();
        grd_get_font_height.Install();
        grd_rect_hook.Install();
        grd_set_color_hook.Install();
        grd_line_hook.Install();
        grd_sphere_hook.Install();
        grd_bbox_aligned_hook.Install();
        grd_bbox_oriented_hook.Install();
        primitive_renderer_begin_deferredHook.Install();

        //Misc hooks
        PlayerDoFrame_hook.Install();
        keen_getBuildVersionString_hook.Install();
        //main_menu_process_hook.Install();

        //mp_menu_process_hook.Install();
        
        xml_parse_hook.Install();
        rfg_init_stage_2_done_hook.Install();
        multi_game_process_remote_console_command_hook.Install(); //Disables rcon handler
        //keen_debug_nativePrint_hook.Install();
        //anim_rig_find_index_hook.Install();
        //animlib_get_loaded_rig_hook.Install();


        keen_getMemoryLayoutDefinition_hook.Install();

        //Export functions for other plugins to use
        FillExports();
        exportedFunctions.push_back({ &ExportInterface, "Reconstructor" });

        ApplyPatches();

        return true;
    }

    //Called when the host dll unloads this plugin
    DLLEXPORT bool __cdecl Reconstructor_PluginShutdown()
    {
        Reconstructor_GlobalState* globalState = GetGlobalState();

        //Remove hooks
        PlayerDoFrame_hook.Remove();
        keen_graphics_beginFrame.Remove();
        keen_graphics_endFrame.Remove();
        grd_string_hook.Remove();
        grd_3d_string_hook.Remove();
        grd_get_string_size_hook.Remove();
        grd_get_font_height.Remove();
        grd_rect_hook.Remove();
        grd_set_color_hook.Remove();
        grd_line_hook.Remove();
        grd_sphere_hook.Remove();
        grd_bbox_aligned_hook.Remove();
        grd_bbox_oriented_hook.Remove();
        primitive_renderer_begin_deferredHook.Remove();
        keen_getBuildVersionString_hook.Remove();
        //main_menu_process_hook.Remove();

        //mp_menu_process_hook.Remove();

        xml_parse_hook.Remove();
        rfg_init_stage_2_done_hook.Remove();
        multi_game_process_remote_console_command_hook.Remove();
        //keen_debug_nativePrint_hook.Remove();
        //anim_rig_find_index_hook.Remove();
        //animlib_get_loaded_rig_hook.Remove();


        keen_getMemoryLayoutDefinition_hook.Install();

        //Relock mouse and camera so game has full control of them and patches are removed
        LockMouse();
        DeactivateFreeCamera();

        //Remove custom WndProc
        SetWindowLongPtr(globalState->gGameWindowHandle, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(globalState->RfgWndProc));

        return true;
    }
}

void RegisterGui(GuiBase* gui)
{
    OverlayGuis::Instance().RegisterGui(gui);
}

void RemoveGui(GuiBase* gui)
{
    OverlayGuis::Instance().RemoveGui(gui);
}

//Fill IReconstructor function interface so other plugins can access functions this one exposes
void FillExports()
{
    ExportInterface.GetGlobalState = &GetGlobalState;
    ExportInterface.GetRfgFunctions = &GetRfgFunctions;
    ExportInterface.GetImGuiContext = &GetImGuiContext;
    ExportInterface.RegisterPrimitiveDrawCallback = &RegisterPrimitiveDrawCallback;
    ExportInterface.RemovePrimitiveDrawCallback = &RemovePrimitiveDrawCallback;
    ExportInterface.RegisterGui = &RegisterGui;
    ExportInterface.RemoveGui = &RemoveGui;
    ExportInterface.AddWndProcCallback = &AddWndProcCallback;
    ExportInterface.ActivateFreeCamera = &ActivateFreeCamera;
    ExportInterface.DeactivateFreeCamera = &DeactivateFreeCamera;
    ExportInterface.ToggleFreeCamera = &ToggleFreeCamera;
    ExportInterface.ShowHud = &ShowHud;
    ExportInterface.HideHud = &HideHud;
    ExportInterface.ToggleHud = &ToggleHud;
    ExportInterface.ShowFog = &ShowFog;
    ExportInterface.HideFog = &HideFog;
    ExportInterface.ToggleFog = &ToggleFog;
}