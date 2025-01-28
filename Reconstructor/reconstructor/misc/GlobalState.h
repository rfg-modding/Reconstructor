#pragma once
#include "common/Typedefs.h"
#include "common/windows/WindowsWrapper.h"
#include "reconstructor/patching/CodePatcher.h"
#include "common/plugins/IHost.h"
#include "rfg/Containers.h"
#include "rfg/Ui.h"
#include "RFGR_Types/rfg/Weapon.h"
#include "common/containers/FixedArray.h"

struct ImFont;
struct player;
struct rfg_camera;
struct world;
struct mempool_base;
struct anim_rig;

//Todo: Try to reduce the usage of this. Though a certain amount of global usage is unavoidable because the game has many global and static variables
//TODO: Document what each RFG global does, if anything. Some are debug/dev values that don't do much in public builds.
class Reconstructor_GlobalState
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

    CodePatcher Patcher;

    WNDPROC RfgWndProc = nullptr;

    bool ImGuiInitialized = false;

    //Whether widgets are visible (does not block input)
    bool WidgetsVisible = true;
    //Whether imgui gui overlay is visible (does block input)
    bool GuiVisible = false;

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

    anim_rig* CustomAnimRigArray = nullptr;
    anim_rig* OriginalAnimRigArray = nullptr;

    inv_item_info* OriginalInventoryItemArray = nullptr;
    inv_item_info* CustomInventoryItemArray = nullptr;
};

extern Reconstructor_GlobalState* GetGlobalState();