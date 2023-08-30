#include "RenderHooks.h"
#include <IconFontCppHeaders/IconsFontAwesome5_c.h>
#include "reconstructor/functions/FunctionsInternal.h"
#include "reconstructor/misc/GlobalState.h"
#include "reconstructor/hooks/WndProc.h"
#include "common/patching/Offset.h"
#include "reconstructor/gui/GuiBase.h"

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxguid.lib")
#include <dxgi.h>
#include <d3dcommon.h>
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_win32.h>
#include <imgui/backends/imgui_impl_dx11.h>
#include <filesystem>
#ifdef COMPILE_IN_PROFILER
#include "tracy/Tracy.hpp"
#endif

//Only used by render hooks
keen::GraphicsSystem* gGraphicsSystem = nullptr;
keen::RenderSwapChain* gSwapChain = nullptr;
ID3D11Device* gD3D11Device = nullptr;
ID3D11DeviceContext* gD3D11Context = nullptr;
IDXGISwapChain* gD3D11Swapchain = nullptr;
ID3D11RenderTargetView* gMainRenderTargetView = nullptr;

namespace fs = std::filesystem;

//Todo: Put this somewhere sane
namespace gui
{
    const ImVec4 SecondaryTextColor(0.2f, 0.7f, 1.0f, 1.00f); //Light blue;
}

void InitImGuiD3D11();

std::vector<ImGuiCallbackFunc> ImGuiCallbacks;
std::vector<OverlayCallbackFunc> OverlayCallbacks;
std::vector<PrimitiveDrawCallbackFunc> PrimitiveDrawCallbacks;

HRESULT __stdcall D3D11_ResizeBuffersHookFunc(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags)
{
    static Reconstructor_GlobalState* globalState = GetGlobalState();

    //Todo: Write this
    bool ShouldReInit = false;
    if (gMainRenderTargetView)
    {
        gMainRenderTargetView->Release();
        gMainRenderTargetView = nullptr;
        ShouldReInit = true;
    }

    HRESULT Result = D3D11_ResizeBuffersHook.CallTarget(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);
    printf("D3D11_ResizeBuffersHook called!\n");
#ifdef DEBUG
    Logger::Log("[D3D11_ResizeBuffersHook]:: BufferCount: {}, Width: {}, Height: {}, NewFormat: {}, SwapChainFlags: {} .... Result: {:#x}\n", BufferCount, Width, Height, NewFormat, SwapChainFlags, (uint)Result);
#endif

    if (ShouldReInit)
    {
        gD3D11Device = gGraphicsSystem->pDevice;
        gD3D11Context = gGraphicsSystem->pImmediateContext;
        gD3D11Swapchain = gGraphicsSystem->pDefaultSwapChain->pSwapChain;

        ID3D11Texture2D* BackBuffer;
        Result = gD3D11Swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(&BackBuffer));
        if (Result != S_OK || !BackBuffer)
            printf("GetBuffer() failed, return value: %d\n", Result);
        //Logger::LogFatalError("GetBuffer() failed, return value: {}\n", Result);

        D3D11_RENDER_TARGET_VIEW_DESC desc = {};
        memset(&desc, 0, sizeof(desc));
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; //Required to avoid rendering issue with overlay. Without this the proper rgb values will not be displayed.
        desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
        Result = gD3D11Device->CreateRenderTargetView(BackBuffer, &desc, &gMainRenderTargetView);
        if (Result != S_OK)
            printf("CreateRenderTargetView() failed, return value: %d\n", Result);
        //Logger::LogFatalError("CreateRenderTargetView() failed, return value: {}\n", Result);

        BackBuffer->Release();
    }
    return Result;
}
FunHook<D3D11_ResizeBuffersHook_Type> D3D11_ResizeBuffersHook{ 0x0, D3D11_ResizeBuffersHookFunc };

FunHook<void* (keen::GraphicsSystem* pGraphicsSystem, keen::RenderSwapChain* pSwapChain)> keen_graphics_beginFrame
{
    0x0086A8A0,
    [](keen::GraphicsSystem* pGraphicsSystem, keen::RenderSwapChain* pSwapChain) -> void*
    {
        static Reconstructor_GlobalState* globalState = GetGlobalState();

        if (gGraphicsSystem != pGraphicsSystem || gSwapChain != pSwapChain)
        {
            if (gGraphicsSystem)
                printf("gGraphicsSystem changed!");
            if (gSwapChain)
                printf("gSwapChain changed!");

            gGraphicsSystem = pGraphicsSystem;
            gSwapChain = pSwapChain;
        }
        if (globalState->ImGuiInitialized)
            return keen_graphics_beginFrame.CallTarget(pGraphicsSystem, pSwapChain);
        if (!ReadyForD3D11Init())
            return keen_graphics_beginFrame.CallTarget(pGraphicsSystem, pSwapChain);

        static u32 framesReady = 0;
        if (framesReady < 600)
        {
            framesReady++;
            return keen_graphics_beginFrame.CallTarget(pGraphicsSystem, pSwapChain);
        }

        //Grab required D3D11 pointers for rendering and set up imgui.
        gGraphicsSystem = pGraphicsSystem;
        gD3D11Device = gGraphicsSystem->pDevice;
        gD3D11Context = gGraphicsSystem->pImmediateContext;
        gD3D11Swapchain = gGraphicsSystem->pDefaultSwapChain->pSwapChain;

        ID3D11Texture2D* BackBuffer;
        HRESULT Result = gD3D11Swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(&BackBuffer));
        if (Result != S_OK)
            printf("GetBuffer() failed, return value: %d\n", Result);

        D3D11_RENDER_TARGET_VIEW_DESC desc = {};
        memset(&desc, 0, sizeof(desc));
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; //Required to avoid rendering issue with overlay. Without this the proper rgb values will not be displayed.
        desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
        Result = gD3D11Device->CreateRenderTargetView(BackBuffer, &desc, &gMainRenderTargetView);
        if (Result != S_OK)
            printf("CreateRenderTargetView() failed, return value: %d\n", Result);

        BackBuffer->Release();

        globalState->RfgWndProc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(globalState->gGameWindowHandle, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(Reconstructor_WndProc)));
        if (!globalState->RfgWndProc)
            printf("Failed to set custom WndProc! Error message: {}\n", GetLastWin32ErrorAsString().c_str());


        InitImGuiD3D11();

        return keen_graphics_beginFrame.CallTarget(pGraphicsSystem, pSwapChain);
    }
};

FunHook<D3D11_PresentHook_Type> D3D11_PresentHook{ 0x0, D3D11_PresentHookFunc };
HRESULT __stdcall D3D11_PresentHookFunc(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
    static Reconstructor_GlobalState* globalState = GetGlobalState();

    if (!globalState->ImGuiInitialized || globalState->Host->PerformingReload)
        return D3D11_PresentHook.CallTarget(pSwapChain, SyncInterval, Flags);

    if (globalState->OverlayActive || globalState->GuiActive)
    {
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        //Gui code here. This is an input blocking overlay
        DrawCustomGui();
        if (globalState->GuiActive)
        {
            //Run imgui callbacks
            for (ImGuiCallbackFunc callback : ImGuiCallbacks)
                callback();
        }
        //Overlay code here, non input blocking
        if (globalState->OverlayActive)
        {
            //Run overlay callbacks
            for (OverlayCallbackFunc callback : OverlayCallbacks)
                callback();
        }

        //Gui->Draw();
        gD3D11Context->OMSetRenderTargets(1, &gMainRenderTargetView, nullptr);
        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    }

    HRESULT result = D3D11_PresentHook.CallTarget(pSwapChain, SyncInterval, Flags);
#ifdef COMPILE_IN_PROFILER
    FrameMark;
#endif

    return result;
}

bool ReadyForD3D11Init()
{
    static Reconstructor_GlobalState* globalState = GetGlobalState();

    if (!gGraphicsSystem || !gGraphicsSystem->pDevice || !gGraphicsSystem->pImmediateContext || !gGraphicsSystem->pDefaultSwapChain
        || !gGraphicsSystem->pDefaultSwapChain->pSwapChain || !gGraphicsSystem->pDefaultSwapChain->pBackBufferRenderTargetView)
        return false;

    game_state state = rfg::Functions.GameseqGetState();
    if (state < 0 || state > 63) //Wait for proper game state to do any graphics stuff to ensure the game is fully initialized
        return false;

    if (!globalState->gGameWindowHandle) //This is required for imgui init which comes at the end of d3d11 init
    {
        if (!gSwapChain || !gSwapChain->windowHandle)
            return false;

        globalState->gGameWindowHandle = gSwapChain->windowHandle;
    }

    return true;
}

void InitImGuiD3D11()
{
    static Reconstructor_GlobalState* globalState = GetGlobalState();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_IsSRGB;
    ImGui::StyleColorsDark();

    ImGui_ImplDX11_Init(gD3D11Device, gD3D11Context);
    const bool RectResult = GetWindowRect(globalState->gGameWindowHandle, &globalState->gWindowRect);
    if (!RectResult)
        printf("GetWindowRect() failed during Reconstructor init! Error message: %s\n", GetLastWin32ErrorAsString().c_str());
    //Logger::LogError("GetWindowRect() failed during script loader init!\n Error message: {}\n", Globals::GetLastWin32ErrorAsString());

#ifdef DEBUG_BUILD
    string FontAwesomeSolidPath = "C:\\Users\\lukem\\source\\repos\\Reconstructor\\assets\\fonts\\fa-solid-900.ttf";
    string DefaultFontPath = "C:\\Users\\lukem\\source\\repos\\Reconstructor\\assets\\fonts\\Ruda-Bold.ttf";
#elif defined DEBUG_BUILD_OPTIMIZED
    string FontAwesomeSolidPath = "C:\\Users\\lukem\\source\\repos\\Reconstructor\\assets\\fonts\\fa-solid-900.ttf";
    string DefaultFontPath = "C:\\Users\\lukem\\source\\repos\\Reconstructor\\assets\\fonts\\Ruda-Bold.ttf";
#else
    string FontAwesomeSolidPath = "./Reconstructor/fonts/fa-solid-900.ttf";
    string DefaultFontPath = "./Reconstructor/fonts/Roboto-Regular.ttf";
#endif

    ImGui_ImplWin32_Init(globalState->gGameWindowHandle);

    ImGui::StyleColorsDark();
    ImGuiStyle* style = &ImGui::GetStyle();
    ImVec4* colors = style->Colors;

    style->WindowPadding = ImVec2(8, 8);
    style->WindowRounding = 0.0f;
    style->FramePadding = ImVec2(5, 5);
    style->FrameRounding = 0.0f;
    style->ItemSpacing = ImVec2(8, 8);
    style->ItemInnerSpacing = ImVec2(8, 6);
    style->IndentSpacing = 25.0f;
    style->ScrollbarSize = 18.0f;
    style->ScrollbarRounding = 0.0f;
    style->GrabMinSize = 12.0f;
    style->GrabRounding = 0.0f;
    style->TabRounding = 0.0f;
    style->ChildRounding = 0.0f;
    style->PopupRounding = 0.0f;

    style->WindowBorderSize = 1.0f;
    style->FrameBorderSize = 1.0f;
    style->PopupBorderSize = 1.0f;

    colors[ImGuiCol_Text] = ImVec4(0.96f, 0.96f, 0.99f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.114f, 0.114f, 0.125f, 1.0f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.106f, 0.106f, 0.118f, 1.0f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.06f, 0.06f, 0.07f, 1.00f);
    colors[ImGuiCol_Border] = ImVec4(0.216f, 0.216f, 0.216f, 1.0f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.161f, 0.161f, 0.176f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.216f, 0.216f, 0.235f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.255f, 0.255f, 0.275f, 1.00f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.157f, 0.157f, 0.157f, 1.0f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.216f, 0.216f, 0.216f, 1.0f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.157f, 0.157f, 0.157f, 1.0f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.157f, 0.157f, 0.157f, 1.0f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.074f, 0.074f, 0.074f, 1.0f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.32f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.42f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.53f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.44f, 0.44f, 0.47f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.44f, 0.44f, 0.47f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.59f, 0.59f, 0.61f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.20f, 0.20f, 0.22f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.44f, 0.44f, 0.47f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.59f, 0.59f, 0.61f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.20f, 0.20f, 0.22f, 1.00f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.44f, 0.44f, 0.47f, 1.00f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.59f, 0.59f, 0.61f, 1.00f);
    colors[ImGuiCol_Separator] = ImVec4(1.00f, 1.00f, 1.00f, 0.20f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.44f, 0.44f, 0.47f, 0.39f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.44f, 0.44f, 0.47f, 0.59f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.00f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.00f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.00f);
    colors[ImGuiCol_Tab] = ImVec4(0.21f, 0.21f, 0.24f, 1.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.23f, 0.514f, 0.863f, 1.0f);
    colors[ImGuiCol_TabActive] = ImVec4(0.23f, 0.514f, 0.863f, 1.0f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.21f, 0.21f, 0.24f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.23f, 0.514f, 0.863f, 1.0f);
    colors[ImGuiCol_DockingPreview] = ImVec4(0.23f, 0.514f, 0.863f, 0.776f);
    colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.114f, 0.114f, 0.125f, 1.0f);
    colors[ImGuiCol_PlotLines] = ImVec4(0.96f, 0.96f, 0.99f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.12f, 1.00f, 0.12f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.23f, 0.51f, 0.86f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.12f, 1.00f, 0.12f, 1.00f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    colors[ImGuiCol_DragDropTarget] = ImVec4(0.91f, 0.62f, 0.00f, 1.00f);
    colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

    const float GlobalFontSize = 17.0f;
    bool DefaultFontLoaded = false;
    if (fs::exists(DefaultFontPath))
    {
        io.Fonts->AddFontFromFileTTF(DefaultFontPath.c_str(), GlobalFontSize);
        DefaultFontLoaded = true;
    }
    else
        io.Fonts->AddFontDefault();

    static const ImWchar IconsRanges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
    ImFontConfig IconsConfig;
    IconsConfig.MergeMode = true;
    IconsConfig.PixelSnapH = true;

    globalState->FontNormal = io.Fonts->AddFontFromFileTTF(FontAwesomeSolidPath.c_str(), GlobalFontSize, &IconsConfig, IconsRanges);

    const float GlobalBigFontSize = 24.0f;
    if (DefaultFontLoaded)
        io.Fonts->AddFontFromFileTTF(DefaultFontPath.c_str(), GlobalBigFontSize);

    globalState->FontBig = io.Fonts->AddFontFromFileTTF(FontAwesomeSolidPath.c_str(), GlobalBigFontSize, &IconsConfig, IconsRanges);

    /*Start of FontLarge loading*/
    float GlobalLargeFontSize = 35.0f;
    if (DefaultFontLoaded)
        io.Fonts->AddFontFromFileTTF(DefaultFontPath.c_str(), GlobalLargeFontSize);

    globalState->FontLarge = io.Fonts->AddFontFromFileTTF(FontAwesomeSolidPath.c_str(), GlobalLargeFontSize, &IconsConfig, IconsRanges);
    /*End of FontLarge loading*/

    /*Start of FontHuge loading*/
    float GlobalHugeFontSize = 70.0f;
    if (DefaultFontLoaded)
        io.Fonts->AddFontFromFileTTF(DefaultFontPath.c_str(), GlobalHugeFontSize);

    globalState->FontHuge = io.Fonts->AddFontFromFileTTF(FontAwesomeSolidPath.c_str(), GlobalHugeFontSize, &IconsConfig, IconsRanges);
    /*End of FontHuge loading*/

    globalState->ImGuiInitialized = true;
    //Hooks::UpdateD3D11Pointers = false;
    printf("ImGui Initialized.\n");
}

void __fastcall primitive_renderer_begin_deferredHook_Func(rl_primitive_renderer* thisPtr)
{
    static Reconstructor_GlobalState* globalState = GetGlobalState();
    primitive_renderer_begin_deferredHook.CallTarget(thisPtr);

    if (!globalState->ImGuiInitialized || globalState->Host->PerformingReload)
        return;

    //Todo: Add helpers to make this easier + less restrictive. Should have some class that keeps a list of primitive render commands and then runs them all here
    //Do all primitive rendering stuff here
    if (!globalState || !globalState->Player || !globalState->MainCamera || !globalState->World)
        return;

    //Run primitive draw callbacks
    for (PrimitiveDrawCallbackFunc callback : PrimitiveDrawCallbacks)
        callback();
}
FunHook<primitive_renderer_begin_deferredHook_Type> primitive_renderer_begin_deferredHook { 0x000F0E50, primitive_renderer_begin_deferredHook_Func };


ImGuiContext* GetImGuiContext()
{
    return ImGui::GetCurrentContext();
}

//ImGui callback handling code
extern void RegisterImGuiCallback(ImGuiCallbackFunc callback)
{
    if (!callback)
        printf("Warning! Attempted to register imgui callback via nullptr to callback.\n");

    for (auto* existingCallback : ImGuiCallbacks)
    {
        if (existingCallback == callback)
            return;
    }

    ImGuiCallbacks.push_back(callback);
}

extern void RemoveImGuiCallback(ImGuiCallbackFunc callback)
{
    i32 callbackIndex = -1;
    for (i32 i = 0; i < ImGuiCallbacks.size(); i++)
    {
        if (ImGuiCallbacks[i] == callback)
        {
            callbackIndex = i;
            break;
        }
    }

    if (callbackIndex != -1)
        ImGuiCallbacks.erase(ImGuiCallbacks.begin() + callbackIndex);
}


//Overlay callback handling code
void RegisterOverlayCallback(OverlayCallbackFunc callback)
{
    if (!callback)
        printf("Warning! Attempted to register imgui callback via nullptr to callback.\n");

    for (auto* existingCallback : OverlayCallbacks)
    {
        if (existingCallback == callback)
            return;
    }

    OverlayCallbacks.push_back(callback);
}

void RemoveOverlayCallback(OverlayCallbackFunc callback)
{
    i32 callbackIndex = -1;
    for (i32 i = 0; i < OverlayCallbacks.size(); i++)
    {
        if (OverlayCallbacks[i] == callback)
        {
            callbackIndex = i;
            break;
        }
    }

    if (callbackIndex != -1)
        OverlayCallbacks.erase(OverlayCallbacks.begin() + callbackIndex);
}


//Primitive draw callback handling code
extern void RegisterPrimitiveDrawCallback(PrimitiveDrawCallbackFunc callback)
{
    if (!callback)
        printf("Warning! Attempted to register primitive draw callback via nullptr to callback.\n");

    for (auto* existingCallback : PrimitiveDrawCallbacks)
    {
        if (existingCallback == callback)
            return;
    }

    PrimitiveDrawCallbacks.push_back(callback);
}

extern void RemovePrimitiveDrawCallback(PrimitiveDrawCallbackFunc callback)
{
    i32 callbackIndex = -1;
    for (i32 i = 0; i < PrimitiveDrawCallbacks.size(); i++)
    {
        if (PrimitiveDrawCallbacks[i] == callback)
        {
            callbackIndex = i;
            break;
        }
    }

    if (callbackIndex != -1)
        PrimitiveDrawCallbacks.erase(PrimitiveDrawCallbacks.begin() + callbackIndex);
}