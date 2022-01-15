#pragma once
#include "common/Typedefs.h"
#include "common/windows/WindowsWrapper.h"
#include "rsl2/patching/SnippetManager.h"
#include "common/plugins/IHost.h"
#include "rfg/Containers.h"
#include "rfg/Ui.h"
#include "RFGR_Types/rfg/Weapon.h"
#include "RFGR_Types/rfg/stream2/stream2.h"
#include "common/containers/FixedArray.h"

struct ImFont;
struct player;
struct rfg_camera;
struct world;
struct mempool_base;
namespace keen
{
    struct Event;
}

//Todo: Try to reduce the usage of this. Though a certain amount of global usage is unavoidable because the game has many global and static variables
//TODO: Document what each RFG global does, if anything. Some are debug/dev values that don't do much in public builds.
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
    stream2_container* Stream_containers = nullptr; //0x016C2B50

    //Whether or not to draw the games built in memory tracking overlay
    bool DrawRfgMemoryTracker = false;
    //Whether or not to draw the RSL debug overlay
    bool DrawRSLDebugOverlay = true;

    //UI list used by RFG
    farray<ui_menu*, 8>* RfgMenusList = nullptr;

    FixedArray<air_bomb_info> AirBombInfos;
    u32* NumAirBombInfos = nullptr;

    FixedArray<weapon_info> WeaponInfos;
    u32* NumWeaponInfos = nullptr;

    bool* LOS_blocker_debug = nullptr;
    bool* Air_bomb_debug = nullptr; //0x01E28601
    bool* Activity_register_damage_debug = nullptr; //0x01E285C8
    bool* Tod_show_sun_path = nullptr; //0x02132928
    bool* Player_hold_debug = nullptr; //0x02C2E0D7
    bool* Salvage_debug = nullptr; //0x02C2E225
    float* Player_max_movement_speed_override = nullptr; //0x02C2E0B4
    float* SpeedScale = nullptr; //0x0125BBD4
    bool* Use_packfiles = nullptr; //0x01958C55
    mempool_base** Xml_mempool = nullptr; //0x018E77DC

    u32* Stream_thread_id = nullptr; //0x016C2B4C
    stream2_container** Stream_q = nullptr; //0x016C2B40
    stream2_container** Stream_q_passive = nullptr; //0x016C2B44
    stream2_container** Stream_q_unload = nullptr; //0x016C2B3C
    bool* Stream_q_paused = nullptr; //0x016C2B48
    bool* Re_entrant = nullptr; //0x01708A04
    bool* Multiplayer = nullptr; //0x02BEA588
    keen::Event* g_stream2ContainerLoadEvent = nullptr; //0x016C2B64
    stream2_container** g_pStream2ContainerToLoad = nullptr; //0x016C2B60
    volatile bool* g_stream2ContainerLoaded = nullptr; //0x016C2B4A
};

extern RSL2_GlobalState* GetGlobalState();