#include "common/windows/WindowsWrapper.h"
#include "common/patching/FunHook.h"
#include "common/plugins/Plugin.h"
#include "common/plugins/IHost.h"
#include "hooks/PlayerDoFrame.h"
#include "hooks/MiscHooks.h"
#include "hooks/XmlHooks.h"
#include "hooks/ui/MainMenuHooks.h"
#include "functions/FunctionsInternal.h"
#include "hooks/GrdRenderHooks.h"
#include "hooks/RenderHooks.h"
#include "hooks/WndProc.h"
#include "misc/GlobalState.h"
#include "common/Typedefs.h"
#include "common/Common.h"
#include "rsl2/patching/Offset.h"
#include "rsl2/hooks/Camera.h"
#include "rsl2/util/Util.h"
#include "rsl2/IRSL2.h"
#ifdef COMPILE_IN_PROFILER
#include "tracy/Tracy.hpp"
#endif
#include <kiero/kiero.h>
#include <cstdio>

//Note: When a plugin is reloaded the host calls RSL2_PluginUnload and then RSL2_PluginInit again
//      as if it were the first time the host were loading the plugin.

IRSL2 ExportInterface;
void FillExports();

//Need to use extern "C" to avoid C++ export name mangling. Lets us use the exact name RSL2_XXXX with GetProcAddress in the host
extern "C"
{
    //Called when a plugin this one depends on is loaded/unloaded. This one has no dependencies so they'll never be called
    DLLEXPORT void __cdecl RSL2_OnDependencyUnload(const string& dependencyName) { } //Called immediately before dependency shutdown + unload
    DLLEXPORT void __cdecl RSL2_OnDependencyLoad(const string& dependencyName) { } //Called immediately after dependency load + init

    //Called before init. Returns info like a dependencies list used to determine plugin load order
    DLLEXPORT PluginInfo __cdecl RSL2_PluginInfo()
    {
        return PluginInfo
        {
            .Dependencies = {}
        };
    }

    //Called when the host dll loads this plugin
    DLLEXPORT bool __cdecl RSL2_PluginInit(IHost* host, std::vector<PluginInterface>& exportedFunctions)
    {
        printf("RSL2.dll RSL2_PluginInit() called!\n");
        RSL2_GlobalState* globalState = GetGlobalState();

        globalState->Host = host;
        globalState->ModuleBase = reinterpret_cast<uintptr_t>(GetModuleHandle(nullptr));
        printf("ModuleBase: %d\n", globalState->ModuleBase);

        InitGlobals();

        if (kiero::init(kiero::RenderType::D3D11) != kiero::Status::Success)
        {
            printf("Error! Failed to init kiero in RSdL2.dll!\n"); 
            
            return false;
        }

        //Todo: Maybe set ModuleBase in FuncHook and provide option to offset function address from it for convenience & less mistakes
        PlayerDoFrame_hook.SetAddr(globalState->ModuleBase + 0x6E6290);
        PlayerDoFrame_hook.Install();
        keen_graphics_beginFrame.SetAddr(globalState->ModuleBase + 0x0086A8A0);
        keen_graphics_beginFrame.Install();

        //Setup pointers to game functions
        InitFunctionPointers();
        InitCameraPatches();

        //D3D11 hooks
        D3D11_ResizeBuffersHook.SetAddr(kiero::getMethodsTable()[13]);
        D3D11_ResizeBuffersHook.Install();
        D3D11_PresentHook.SetAddr(kiero::getMethodsTable()[8]);
        D3D11_PresentHook.Install();

        grd_string_hook.SetAddr(globalState->ModuleBase + 0x000B8DA0);
        grd_string_hook.Install();
        grd_3d_string_hook.SetAddr(globalState->ModuleBase + 0x000B8DE0);
        grd_3d_string_hook.Install();
        grd_get_string_size_hook.SetAddr(globalState->ModuleBase + 0x000B8DF0);
        grd_get_string_size_hook.Install();
        grd_get_font_height.SetAddr(globalState->ModuleBase + 0x000B8E30);
        grd_get_font_height.Install();
        grd_rect_hook.SetAddr(globalState->ModuleBase + 0x000B8E40);
        grd_rect_hook.Install();
        grd_set_color_hook.SetAddr(globalState->ModuleBase + 0x000B8E50);
        grd_set_color_hook.Install();
        grd_line_hook.SetAddr(globalState->ModuleBase + 0x000B8EB0);
        grd_line_hook.Install();
        grd_sphere_hook.SetAddr(globalState->ModuleBase + 0x000B8EC0);
        grd_sphere_hook.Install();
        grd_bbox_aligned_hook.SetAddr(globalState->ModuleBase + 0x000B8F00);
        grd_bbox_aligned_hook.Install();
        grd_bbox_oriented_hook.SetAddr(globalState->ModuleBase + 0x000B8F20);
        grd_bbox_oriented_hook.Install();

        primitive_renderer_begin_deferredHook.SetAddr(globalState->ModuleBase + 0x000F0E50);
        primitive_renderer_begin_deferredHook.Install();

        keen_getBuildVersionString_hook.SetAddr(globalState->ModuleBase + 0x00058740);
        keen_getBuildVersionString_hook.Install();

        main_menu_process_hook.SetAddr(globalState->ModuleBase + 0x00513770);
        main_menu_process_hook.Install();

        xml_parse_hook.SetAddr(globalState->ModuleBase + 0x001CD2F0);
        xml_parse_hook.Install();

        rfg_init_stage_2_done_hook.SetAddr(globalState->ModuleBase + 0x001D55C0);
        rfg_init_stage_2_done_hook.Install();

        //Set export functions for this plugin
        FillExports();
        exportedFunctions.push_back({ &ExportInterface, "RSL2" });
        return true;
    }

    //Called when the host dll unloads this plugin
    DLLEXPORT bool __cdecl RSL2_PluginShutdown()
    {
        printf("RSL2.dll RSL2_PluginShutdown() called!\n");
        RSL2_GlobalState* globalState = GetGlobalState();

        kiero::shutdown();

        //Remove hooks
        PlayerDoFrame_hook.Remove();
        keen_graphics_beginFrame.Remove();
        D3D11_ResizeBuffersHook.Remove();
        D3D11_PresentHook.Remove();
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
        main_menu_process_hook.Remove();
        xml_parse_hook.Remove();
        rfg_init_stage_2_done_hook.Remove();

        //Relock mouse and camera so game has full control of them and patches are removed
        LockMouse();
        LockCamera();

        //Remove custom WndProc
        SetWindowLongPtr(globalState->gGameWindowHandle, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(globalState->RfgWndProc));
        return true;
    }
}

//Fill IRSL2 function interface so other plugins can access functions this one exposes
void FillExports()
{
    ExportInterface.GetGlobalState = &GetGlobalState;
    ExportInterface.GetRfgFunctions = &GetRfgFunctions;
    ExportInterface.GetImGuiContext = &GetImGuiContext;
    ExportInterface.RegisterImGuiCallback = &RegisterImGuiCallback;
    ExportInterface.RemoveImGuiCallback = &RemoveImGuiCallback;
    ExportInterface.RegisterOverlayCallback = &RegisterOverlayCallback;
    ExportInterface.RemoveOverlayCallback = &RemoveOverlayCallback;
    ExportInterface.RegisterPrimitiveDrawCallback = &RegisterPrimitiveDrawCallback;
    ExportInterface.RemovePrimitiveDrawCallback = &RemovePrimitiveDrawCallback;
}