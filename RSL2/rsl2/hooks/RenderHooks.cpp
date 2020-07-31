#include "RenderHooks.h"
#include "rsl2/misc/GlobalState.h"
#include "rsl2/hooks/WndProc.h"
#include "rsl2/functions/Functions.h"
#include "rsl2/rfg/keen/GraphicsSystem.h"
#include "rsl2/rfg/Game.h"
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxguid.lib")
#include <dxgi.h>
#include <d3dcommon.h>
#include <imgui/imgui.h>
#include <imgui/examples/imgui_impl_win32.h>
#include <imgui/examples/imgui_impl_dx11.h>
#include <IconFontCppHeaders/IconsFontAwesome5.h>
#include <filesystem>
#ifdef COMPILE_IN_PROFILER
#include "tracy/Tracy.hpp"
#endif

#include "rsl2/rfg/Player.h"
#include "rsl2/rfg/Human.h"
#include "rsl2/rfg/Object.h"
#include "rsl2/rfg/Camera.h"
#include "rsl2/rfg/World.h"

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

void ShowExampleAppSimpleOverlay(bool* p_open);
void InitImGuiD3D11();

HRESULT __stdcall D3D11_ResizeBuffersHookFunc(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags)
{
    static RSL2_GlobalState* globalState = GetGlobalState();

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
        if (Result != S_OK)
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
        static RSL2_GlobalState* globalState = GetGlobalState();

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

        globalState->RfgWndProc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(globalState->gGameWindowHandle, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(RSL2_WndProc)));
        if (!globalState->RfgWndProc)
            printf("Failed to set custom WndProc! Error message: {}\n", GetLastWin32ErrorAsString().c_str());


        InitImGuiD3D11();

        return keen_graphics_beginFrame.CallTarget(pGraphicsSystem, pSwapChain);
    }
};

FunHook<D3D11_PresentHook_Type> D3D11_PresentHook{ 0x0, D3D11_PresentHookFunc };
HRESULT __stdcall D3D11_PresentHookFunc(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
    static RSL2_GlobalState* globalState = GetGlobalState();

    if (!globalState->ImGuiInitialized)
        return D3D11_PresentHook.CallTarget(pSwapChain, SyncInterval, Flags);
    //if (ScriptLoaderCloseRequested)
    //    return D3D11_PresentHook.CallTarget(pSwapChain, SyncInterval, Flags);

    //static auto Gui = IocContainer->resolve<IGuiManager>();
    //if (Gui && Gui->Ready())
    if (globalState->OverlayActive || globalState->GuiActive)
    {
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        //Gui code here. This is an input blocking overlay
        if (globalState->GuiActive)
        {
            ImGui::ShowDemoWindow();
        }
        //Overlay code here, non input blocking
        if (globalState->OverlayActive)
        {
            ShowExampleAppSimpleOverlay(&globalState->OverlayActive);
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
    static RSL2_GlobalState* globalState = GetGlobalState();

    if (!gGraphicsSystem || !gGraphicsSystem->pDevice || !gGraphicsSystem->pImmediateContext || !gGraphicsSystem->pDefaultSwapChain
        || !gGraphicsSystem->pDefaultSwapChain->pSwapChain || !gGraphicsSystem->pDefaultSwapChain->pBackBufferRenderTargetView)
        return false;

    game_state state = rfg::GameseqGetState();
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

void ShowExampleAppSimpleOverlay(bool* p_open)
{
    static RSL2_GlobalState* globalState = GetGlobalState();

    // FIXME-VIEWPORT: Select a default viewport
    const float DISTANCE = 10.0f;
    static int corner = 0;
    ImGuiIO& io = ImGui::GetIO();
    if (corner != -1)
    {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImVec2 work_area_pos = viewport->GetWorkPos();   // Instead of using viewport->Pos we use GetWorkPos() to avoid menu bars, if any!
        ImVec2 work_area_size = viewport->GetWorkSize();
        ImVec2 window_pos = ImVec2((corner & 1) ? (work_area_pos.x + work_area_size.x - DISTANCE) : 
            (work_area_pos.x + DISTANCE), (corner & 2) ? (work_area_pos.y + work_area_size.y - DISTANCE) : (work_area_pos.y + DISTANCE));
        ImVec2 window_pos_pivot = ImVec2((corner & 1) ? 1.0f : 0.0f, (corner & 2) ? 1.0f : 0.0f);
        ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
        ImGui::SetNextWindowViewport(viewport->ID);
    }
    ImGui::SetNextWindowBgAlpha(0.5f); // Transparent background
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize 
                                    | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
    if (corner != -1)
        window_flags |= ImGuiWindowFlags_NoMove;

    if (ImGui::Begin("Example: Simple overlay", p_open, window_flags))
    {
        ImGui::Text("FPS: %.2f", io.Framerate);
        ImGui::Separator();
        if (globalState->Player)
            ImGui::Text("Player Position: (%.2f,%.2f, %.2f)", globalState->Player->pos.x, globalState->Player->pos.y, globalState->Player->pos.z);
        else
            ImGui::Text("Player Position: <invalid>");

        if (ImGui::BeginPopupContextWindow())
        {
            if (ImGui::MenuItem("Custom", NULL, corner == -1)) corner = -1;
            if (ImGui::MenuItem("Top-left", NULL, corner == 0)) corner = 0;
            if (ImGui::MenuItem("Top-right", NULL, corner == 1)) corner = 1;
            if (ImGui::MenuItem("Bottom-left", NULL, corner == 2)) corner = 2;
            if (ImGui::MenuItem("Bottom-right", NULL, corner == 3)) corner = 3;
            if (p_open && ImGui::MenuItem("Close")) *p_open = false;
            ImGui::EndPopup();
        }
    }
    ImGui::End();
}

void InitImGuiD3D11()
{
    static RSL2_GlobalState* globalState = GetGlobalState();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::StyleColorsDark();

    ImGui_ImplDX11_Init(gD3D11Device, gD3D11Context);
    const bool RectResult = GetWindowRect(globalState->gGameWindowHandle, &globalState->gWindowRect);
    if (!RectResult)
        printf("GetWindowRect() failed during RSL2 init! Error message: %s\n", GetLastWin32ErrorAsString().c_str());
    //Logger::LogError("GetWindowRect() failed during script loader init!\n Error message: {}\n", Globals::GetLastWin32ErrorAsString());

#ifdef DEBUG_BUILD
    string FontAwesomeSolidPath = "C:\\Users\\moneyl\\source\\repos\\RSL2\\assets\\fonts\\fa-solid-900.ttf";
    string DefaultFontPath = "C:\\Users\\moneyl\\source\\repos\\RSL2\\assets\\fonts\\Ruda-Bold.ttf";
#elif defined DEBUG_BUILD_OPTIMIZED
    string FontAwesomeSolidPath = "C:\\Users\\moneyl\\source\\repos\\RSL2\\assets\\fonts\\fa-solid-900.ttf";
    string DefaultFontPath = "C:\\Users\\moneyl\\source\\repos\\RSL2\\assets\\fonts\\Ruda-Bold.ttf";
#else
    string FontAwesomeSolidPath = "./RSL/fonts/fa-solid-900.ttf");
    string DefaultFontPath = "./RSL/fonts/Roboto-Regular.ttf");
#endif

    ImGui_ImplWin32_Init(globalState->gGameWindowHandle);


    ImGui::StyleColorsDark();

    ImGui::GetStyle().FrameRounding = 4.0f;
    ImGui::GetStyle().GrabRounding = 4.0f;

    ImVec4* colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_Text] = ImVec4(0.95f, 0.96f, 0.98f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.36f, 0.42f, 0.47f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
    colors[ImGuiCol_Border] = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.12f, 0.20f, 0.28f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.09f, 0.12f, 0.14f, 1.00f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.09f, 0.12f, 0.14f, 0.65f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.39f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.18f, 0.22f, 0.25f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.09f, 0.21f, 0.31f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.37f, 0.61f, 1.00f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.20f, 0.25f, 0.29f, 0.55f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_Separator] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    colors[ImGuiCol_Tab] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
    colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
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
    static RSL2_GlobalState* globalState = GetGlobalState();
    primitive_renderer_begin_deferredHook.CallTarget(thisPtr);

    if (!globalState->ImGuiInitialized)
        return;

    //Todo: Add helpers to make this easier + less restrictive. Should have some class that keeps a list of primitive render commands and then runs them all here
    //Do all primitive rendering stuff here

    if (!globalState || !globalState->Player || !globalState->MainCamera || !globalState->World)
        return;

    //Test out rfg primitive rendering code
    static gr_state renderState;
    static bool firstRun = true;
    if (firstRun)
    {
        rfg::gr_state_constructor(&renderState, nullptr, ALPHA_BLEND_ADDITIVE, CLAMP_MODE_CLAMP, ZBUF_NONE, STENCIL_NONE, 0, CULL_MODE_CULL, TNL_CLIP_MODE_NONE, TNL_LIGHT_MODE_NONE);
        firstRun = false;
    }
    //if (globalState->Player)
    {

        //String over masons head
        //vector stringPos = (globalState->Player->last_known_bmin + ((globalState->Player->last_known_bmax - globalState->Player->last_known_bmin).Scale(0.5f))) + vector(0.0f, 1.5f, 0.0f);
        vector stringPos = globalState->Player->pos;// -(globalState->MainCamera->real_orient.rvec.Magnitude() * 5.0f);
        stringPos += vector(0.0f, 2.0f, 0.0f);
        vector stringOffset = (globalState->Player->last_known_bmax - globalState->Player->last_known_bmin).Scale(-0.5f);;
        stringPos.x += stringOffset.x;
        stringPos.z += stringOffset.z;
        matrix stringOrient = globalState->MainCamera->real_orient;
        
        //stringOrient.fvec.y = 0.0f;
        //stringOrient.rvec.y = 0.0f;
        //stringPos += globalState->MainCamera->real_orient.fvec * 2.5f;
        //vector stringOffset = (globalState->MainCamera->real_orient.rvec.UnitVector() * -3.0f);
        //stringPos.x += stringOffset.x;
        //stringPos.z += stringOffset.z;
        string positionString = globalState->Player->pos.GetDataString(true, false);
        int fontNum = 0;

        //rfg::gr_bbox_aligned(&globalState->Player->last_known_bmin, &globalState->Player->last_known_bmax, &renderState);
        //Todo: Use camera orientation instead
        rfg::gr_3d_string(&stringPos, &stringOrient, 0.002f, positionString.c_str(), fontNum, &renderState);
    }
    //if (globalState->World)
    {
        //Todo: Fix range based iterators for base_array<T>
        for (u32 i = 0; i < globalState->World->all_objects.Size(); i++)
        {
            Object* object = globalState->World->all_objects[i];

            if (!object || object->obj_type != OT_HUMAN || object == globalState->Player)
                continue;

            Human* human = reinterpret_cast<Human*>(object);

            float maxDrawDistance = 100.0f;
            vector distance = object->pos - globalState->MainCamera->real_pos;
            if (distance.Magnitude() <= maxDrawDistance)
            {
                //Draw string over humans head + draw their bounding box
                vector stringPos = object->pos;// -(globalState->MainCamera->real_orient.rvec.Magnitude() * 5.0f);
                stringPos += vector(0.0f, 2.0f, 0.0f);
                vector stringOffset = (object->last_known_bmax - object->last_known_bmin).Scale(-0.5f);;
                stringPos.x += stringOffset.x;
                stringPos.z += stringOffset.z;
                matrix stringOrient = globalState->MainCamera->real_orient;

                string positionString = "Health: " + std::to_string(human->hit_points) + "/" + std::to_string(human->max_hit_points);
                int fontNum = 0;

                rfg::gr_bbox_aligned(&object->last_known_bmin, &object->last_known_bmax, &renderState);
                rfg::gr_3d_string(&stringPos, &stringOrient, 0.003f, positionString.c_str(), fontNum, &renderState);

                u32 maxPathNodeCount = 30;
                //Draw pathfinding line of human
                if (human->pf.path)
                {
                    path_node* curNode = human->pf.path;
                    path_node* nextNode = curNode->next;
                    u32 nodeCount = 1;
                    while (nextNode && nodeCount < maxPathNodeCount)
                    {
                        rfg::gr_3d_line(&curNode->pos, &nextNode->pos, &renderState);
                        curNode = nextNode;
                        nextNode = curNode->next;
                        nodeCount++;
                    }
                }
            }
        }
    }
}
FunHook<primitive_renderer_begin_deferredHook_Type> primitive_renderer_begin_deferredHook { 0x0, primitive_renderer_begin_deferredHook_Func };