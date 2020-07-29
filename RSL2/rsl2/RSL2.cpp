#include "common/Typedefs.h"
#include "common/windows/WindowsWrapper.h"
#include "common/patching/FunHook.h"
#include "common/Common.h"
#include <cstdio>
#include <filesystem>

//Todo: Have common lib with RFG types instead of tossing them everywhere
#include "rfg/Player.h"
#include "rfg/keen/GraphicsSystem.h"
#include "rfg/Game.h"

#include <imgui/imgui.h>
#include <imgui/examples/imgui_impl_win32.h>
#include <imgui/examples/imgui_impl_dx11.h>
#include <IconFontCppHeaders/IconsFontAwesome5.h>
//#include "imgui_stdlib.h"

#include <kiero/kiero.h>

//Note: When a plugin is reloaded the host called RSL2_PluginUnload and then RSL2_PluginInit agai
//      as if it were the first time the host were loading the plugin.

static uintptr_t ModuleBase = 0;

bool ImGuiInitialized = false;
keen::GraphicsSystem* gGraphicsSystem = nullptr;
ID3D11Device* gD3D11Device = nullptr;
ID3D11DeviceContext* gD3D11Context = nullptr;
IDXGISwapChain* gD3D11Swapchain = nullptr;
ID3D11RenderTargetView* gMainRenderTargetView = nullptr;

ImFont* FontNormal = nullptr;
ImFont* FontBig = nullptr;
ImFont* FontLarge = nullptr;
ImFont* FontHuge = nullptr;

HWND gGameWindowHandle = nullptr;
RECT gWindowRect = { 0, 0, 0, 0 };

bool ReadyForD3D11Init();
void InitImGuiD3D11();
void WaitForValidGameState();
HWND GetTopWindowHandle();

namespace fs = std::filesystem;

FunHook<void(Player*)> PlayerDoFrame_hook
{
    0x6E6290, //__cdecl
    [](Player* player)
    {
        Object* playerObj = reinterpret_cast<Object*>(player);
        static unsigned int counter = 0;

        //playerObj->obj_flags.render_flags = 0;
        if (counter % 60 == 0)
        {
            printf("Player position: %s\n", playerObj->pos.GetDataString(true, true).c_str());
            printf("Player render_flags: %d\n", playerObj->obj_flags.render_flags);
        }
        counter++;
        PlayerDoFrame_hook.CallTarget(player);
    }
};

//keen::GraphicsCommandBuffer *__cdecl keen::graphics::beginFrame(keen::GraphicsSystem *pGraphicsSystem, keen::RenderSwapChain *pSwapChain)
FunHook<void*(keen::GraphicsSystem* pGraphicsSystem, keen::RenderSwapChain* pSwapChain)> keen_graphics_beginFrame
{
    0x0086A8A0,
    [](keen::GraphicsSystem* pGraphicsSystem, keen::RenderSwapChain* pSwapChain) -> void*
    {
        if (gGraphicsSystem != pGraphicsSystem)
        {
            if(gGraphicsSystem)
                printf("gGraphicsSystem changed!");
            
            gGraphicsSystem = pGraphicsSystem;
        }
        if (ImGuiInitialized)
            return keen_graphics_beginFrame.CallTarget(pGraphicsSystem, pSwapChain);
        if(!ReadyForD3D11Init())
            return keen_graphics_beginFrame.CallTarget(pGraphicsSystem, pSwapChain);

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
        InitImGuiD3D11();
        
        return keen_graphics_beginFrame.CallTarget(pGraphicsSystem, pSwapChain);
    }
};

using D3D11_ResizeBuffersHook_Type = HRESULT __stdcall(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);
extern FunHook<D3D11_ResizeBuffersHook_Type> D3D11_ResizeBuffersHook;
HRESULT __stdcall D3D11_ResizeBuffersHookFunc(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags)
{
    //Todo: Write this
    bool ShouldReInit = false;
    if (gMainRenderTargetView)
    {
        gMainRenderTargetView->Release();
        gMainRenderTargetView = nullptr;
        ShouldReInit = true;
    }

    HRESULT Result = D3D11_ResizeBuffersHook.CallTarget(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);
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

using D3D11_PresentHook_Type = HRESULT __stdcall(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
extern FunHook<D3D11_PresentHook_Type> D3D11_PresentHook;
HRESULT __stdcall D3D11_PresentHookFunc(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
    if (!ImGuiInitialized)
        return D3D11_PresentHook.CallTarget(pSwapChain, SyncInterval, Flags);
    //if (ScriptLoaderCloseRequested)
    //    return D3D11_PresentHook.CallTarget(pSwapChain, SyncInterval, Flags);

    //static auto Gui = IocContainer->resolve<IGuiManager>();
    //if (Gui && Gui->Ready())
    {
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        ImGui::ShowDemoWindow();

        //Gui->Draw();
        gD3D11Context->OMSetRenderTargets(1, &gMainRenderTargetView, nullptr);
        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    }
    return D3D11_PresentHook.CallTarget(pSwapChain, SyncInterval, Flags);
}
FunHook<D3D11_PresentHook_Type> D3D11_PresentHook{ 0x0, D3D11_PresentHookFunc };


//Todo: Write basic gui test + gui toggling + mouse & keyboard input
//Todo: Organize this code into separate files in a reasonable way

bool ReadyForD3D11Init()
{
    if (gGraphicsSystem && gGraphicsSystem->pDevice && gGraphicsSystem->pImmediateContext && gGraphicsSystem->pDefaultSwapChain)
        if (gGraphicsSystem->pDefaultSwapChain->pSwapChain && gGraphicsSystem->pDefaultSwapChain->pBackBufferRenderTargetView)
            return true;

    return false;
}

void InitImGuiD3D11()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::StyleColorsDark();

    ImGui_ImplDX11_Init(gD3D11Device, gD3D11Context);
    const bool RectResult = GetWindowRect(gGameWindowHandle, &gWindowRect);
    if (!RectResult)
        printf("GetWindowRect() failed during RSL2 init!\n");
        //Logger::LogError("GetWindowRect() failed during script loader init!\n Error message: {}\n", Globals::GetLastWin32ErrorAsString());

#ifdef DEBUG_BUILD
    string FontAwesomeSolidPath = "C:\\Users\\moneyl\\source\\repos\\RSL2\\assets\\Fonts\\fa-solid-900.ttf";
    string DefaultFontPath = "C:\\Users\\moneyl\\source\\repos\\RSL2\\assets\\Fonts\\Ruda-Bold.ttf";
#elif defined DEBUG_BUILD_OPTIMIZED
    string FontAwesomeSolidPath = "C:\\Users\\moneyl\\source\\repos\\RSL2\\assets\\Fonts\\fa-solid-900.ttf";
    string DefaultFontPath = "C:\\Users\\moneyl\\source\\repos\\RSL2\\assets\\Fonts\\Ruda-Bold.ttf";
#else
    string FontAwesomeSolidPath = "./RSL/Fonts/fa-solid-900.ttf");
    string DefaultFontPath = "./RSL/Fonts/Roboto-Regular.ttf");
#endif

    ImGui_ImplWin32_Init(gGameWindowHandle);


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

    FontNormal = io.Fonts->AddFontFromFileTTF(FontAwesomeSolidPath.c_str(), GlobalFontSize, &IconsConfig, IconsRanges);

    const float GlobalBigFontSize = 24.0f;
    if (DefaultFontLoaded)
        io.Fonts->AddFontFromFileTTF(DefaultFontPath.c_str(), GlobalBigFontSize);

    FontBig = io.Fonts->AddFontFromFileTTF(FontAwesomeSolidPath.c_str(), GlobalBigFontSize, &IconsConfig, IconsRanges);

    /*Start of FontLarge loading*/
    float GlobalLargeFontSize = 35.0f;
    if (DefaultFontLoaded)
        io.Fonts->AddFontFromFileTTF(DefaultFontPath.c_str(), GlobalLargeFontSize);

    FontLarge = io.Fonts->AddFontFromFileTTF(FontAwesomeSolidPath.c_str(), GlobalLargeFontSize, &IconsConfig, IconsRanges);
    /*End of FontLarge loading*/

    /*Start of FontHuge loading*/
    float GlobalHugeFontSize = 70.0f;
    if (DefaultFontLoaded)
        io.Fonts->AddFontFromFileTTF(DefaultFontPath.c_str(), GlobalHugeFontSize);

    FontHuge = io.Fonts->AddFontFromFileTTF(FontAwesomeSolidPath.c_str(), GlobalHugeFontSize, &IconsConfig, IconsRanges);
    /*End of FontHuge loading*/

    ImGuiInitialized = true;
    //Hooks::UpdateD3D11Pointers = false;
    printf("ImGui Initialized.\n");
}

//Todo: Organize this properly
namespace rfg
{
    //game_state __cdecl gameseq_set_state(game_state state, bool uninterruptible)
    using F_GameseqGetState = game_state(__cdecl*)();
    F_GameseqGetState GameseqGetState;

    //void __cdecl gameseq_set_state(game_state state, bool uninterruptible)
    using F_GameseqSetState = void(__cdecl*)(game_state state, bool uninterruptible);
    F_GameseqSetState GameseqSetState;
}

//Todo: Organize this properly
template<typename T>
void RegisterFunction(T& Function, DWORD RelativeAddress)
{
    Function = reinterpret_cast<T>(ModuleBase + RelativeAddress);
}

void WaitForValidGameState()
{
    game_state RFGRState = rfg::GameseqGetState();
    auto StartTime = std::chrono::steady_clock::now();
    auto EndTime = std::chrono::steady_clock::now();
    long long TimeElapsed = 0LL;
    do
    {
        TimeElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(EndTime - StartTime).count();
        if (TimeElapsed > 1000LL) //Todo: Figure out if casting 1000 as a long long is necessary in this case or ever.
        {
            RFGRState = rfg::GameseqGetState();
            StartTime = EndTime;
            printf("Current RFGR State: %d\n", (u32)RFGRState);
            //Logger::Log("Current RFGR State: {}\n", magic_enum::enum_name(RFGRState));
        }
        EndTime = std::chrono::steady_clock::now();
    } while (RFGRState < 0 || RFGRState > 63);
}

HWND GetTopWindowHandle()
{
    std::pair<HWND, DWORD> params = { nullptr, GetCurrentProcessId() };

    // Enumerate the windows using a lambda to process each window
    const BOOL bResult = EnumWindows([](const HWND hwnd, LPARAM lParam) -> BOOL
        {
            const auto pParams = reinterpret_cast<std::pair<HWND, DWORD>*>(lParam);

            DWORD processId;
            if (GetWindowThreadProcessId(hwnd, &processId) && processId == pParams->second)
            {
                // Stop enumerating
                SetLastError(-1);
                pParams->first = hwnd;
                return FALSE;
            }

            // Continue enumerating
            return TRUE;
        }, reinterpret_cast<LPARAM>(&params));

    if (!bResult && GetLastError() == -1 && params.first)
    {
        return params.first;
    }

    return nullptr;
}

//Need to use extern "C" to avoid C++ export name mangling. Lets us use the exact name RSL2_XXXX with GetProcAddress in the host
extern "C"
{
    //Called when the host dll loads this plugin
    DLLEXPORT bool __cdecl RSL2_PluginInit()
    {
        printf("RSL2.dll RSL2_PluginInit() called!\n");

        ModuleBase = reinterpret_cast<uintptr_t>(GetModuleHandle(nullptr));
        printf("ModuleBase: %d\n", ModuleBase);

        if (kiero::init(kiero::RenderType::D3D11) != kiero::Status::Success)
        {
            printf("Error! Failed to init kiero in RSL2.dll!\n");
            return false;
        }

        //Todo: Maybe set ModuleBase in FuncHook and provide option to offset function address from it for convenience & less mistakes
        PlayerDoFrame_hook.SetAddr(ModuleBase + 0x6E6290);
        PlayerDoFrame_hook.Install();
        keen_graphics_beginFrame.SetAddr(ModuleBase + 0x0086A8A0);
        keen_graphics_beginFrame.Install();

        //Todo: Wait for valid game state then get window handle
        RegisterFunction(rfg::GameseqGetState, 0x003BFC70);
        RegisterFunction(rfg::GameseqSetState, 0x003D8730);
        //WaitForValidGameState();
        gGameWindowHandle = GetTopWindowHandle();

        //D3D11 hooks
        D3D11_ResizeBuffersHook.SetAddr(kiero::getMethodsTable()[13]);
        D3D11_ResizeBuffersHook.Install();
        D3D11_PresentHook.SetAddr(kiero::getMethodsTable()[8]);
        D3D11_PresentHook.Install();

        return true;
    }

    //Called when the host dll unloads this plugin
    DLLEXPORT bool __cdecl RSL2_PluginUnload()
    {
        printf("RSL2.dll RSL2_PluginUnload() called!\n");
        
        PlayerDoFrame_hook.Remove();

        return true;
    }
}