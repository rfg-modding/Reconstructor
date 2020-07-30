#pragma once
#include "common/Typedefs.h"
#include "common/windows/WindowsWrapper.h"
#include "rsl2/patching/SnippetManager.h"

struct ImFont;

//Todo: Try to eliminate this and if not possible reduce it's usage as much as possible. A big file with tons of global data was a design flaw of RSL1
class RSL2_GlobalState
{
public:
    uintptr_t ModuleBase = 0;
    ImFont* FontNormal = nullptr;
    ImFont* FontBig = nullptr;
    ImFont* FontLarge = nullptr;
    ImFont* FontHuge = nullptr;

    HWND gGameWindowHandle = nullptr;
    RECT gWindowRect = { 0, 0, 0, 0 };

    SnippetManager SnippetManager;

    WNDPROC RfgWndProc = nullptr;

    bool ImGuiInitialized = false;

    //Whether overlay is active (does not block input)
    bool OverlayActive = true;
    //Whether imgui gui overlay is active (does block input)
    bool GuiActive = false;

    //Addresses of some locations that are patched to control mouse movement + visibility
    DWORD MouseGenericPollMouseVisibleAddress = 0;
    DWORD CenterMouseCursorCallAddress = 0;
};

extern RSL2_GlobalState* GetGlobalState();