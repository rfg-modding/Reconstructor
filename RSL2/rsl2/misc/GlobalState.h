#pragma once
#include "common/Typedefs.h"
#include "common/windows/WindowsWrapper.h"
#include "rsl2/patching/SnippetManager.h"
#include "common/plugins/IHost.h"
#include "rfg/Containers.h"
#include "rfg/Ui.h"
#include "RFGR_Types/rfg/Weapon.h"
#include "common/containers/FixedArray.h"

struct ImFont;
struct player;
struct rfg_camera;
struct world;

//Todo: Try to eliminate this and if not possible reduce it's usage as much as possible. A big file with tons of global data was a design flaw of RSL1
class RSL2_GlobalState
{
public:
    IHost* Host = nullptr;

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

    player* Player = nullptr;
    rfg_camera* MainCamera = nullptr;
    world* World = nullptr;

    //Whether or not to draw the games built in memory tracking overlay
    bool DrawRfgMemoryTracker = false;
    //Whether or not to draw the RSL debug overlay
    bool DrawRSLDebugOverlay = true;

    //UI list used by RFG
    farray<ui_menu*, 8>* RfgMenusList = nullptr;

    FixedArray<air_bomb_info> AirBombInfos;
    u32* NumAirBombInfos = nullptr;

    bool* LOS_blocker_debug = nullptr;
    bool* Air_bomb_debug = nullptr; //0x01E28601
    bool* Activity_register_damage_debug = nullptr; //0x01E285C8
    bool* Tod_show_sun_path = nullptr; //0x02132928
    bool* Player_hold_debug = nullptr; //0x02C2E0D7
    bool* Salvage_debug = nullptr; //0x02C2E225
};

extern RSL2_GlobalState* GetGlobalState();