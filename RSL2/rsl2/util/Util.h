#pragma once
#include "rsl2/misc/GlobalState.h"
#include "rsl2/functions/FunctionsInternal.h"
#include "rsl2/patching/Offset.h"

static std::string CharArrayToString(char* Array, int Size)
{
    std::string String;
    for (int i = 0; i < Size; i++)
    {
        if (Array[i] == '\0')
        {
            break; //Stop at null terminator. Including null characters can mess up string comparisons elsewhere.
        }
        String += Array[i];
    }
    return String;
}

static std::string CharArrayToString(const char* Array, int Size)
{
    return CharArrayToString(const_cast<char*>(Array), Size);
}

//Attempts to disable the main menu buttons for disabled features such as MP, LAN, and Wrecking Crew.
static void TryHideInvalidMainMenuOptions()
{
    RSL2_GlobalState* globalState = GetGlobalState();
    if (!globalState->RfgMenusList || !(*globalState->RfgMenusList)[0])
        return;

    ui_menu& Menu = *(*globalState->RfgMenusList)[0];
    if (Menu.m_num_items == 9)
    {
        if (Menu.m_items[0])
        {
            if (CharArrayToString(Menu.m_items[0]->m_caption, 128) == "MENU_NEW_GAME") //The main menu doesn't have a title, so we have to look for it's buttons
            {
                ui_menu_item_base& WreckingCrewButton = *Menu.m_items[2];
                WreckingCrewButton.m_enabled = false;
                WreckingCrewButton.m_visible = false;

                ui_menu_item_base& MultiplayerButton = *Menu.m_items[3];
                MultiplayerButton.m_enabled = false;
                MultiplayerButton.m_visible = false;

                ui_menu_item_base& LanCrewButton = *Menu.m_items[5];
                LanCrewButton.m_enabled = false;
                LanCrewButton.m_visible = false;
            }
        }
    }
}

static void ReloadXtbls()
{
    RSL2_GlobalState* globalState = GetGlobalState();
    rfg::RfgFunctions* functions = GetRfgFunctions();
    if (!globalState || !functions || !globalState->WeaponInfos.GetRawPointer())
        return;

    //Reset weapon flags and explosions since weapons_read_table() doesn't
    auto& weaponInfos = globalState->WeaponInfos;
    for (u32 i = 0; i < weaponInfos.Size(); i++)
    {
        weapon_info& info = weaponInfos[i];
        memset(&info.flags, 0, sizeof(weapon_info_flags));
        info.m_explosion_info = nullptr;
        info.m_ai_explosion_info = nullptr;
    }
    //Todo: Test all other weapon values to see if weapons_read_table() has any other it doesn't properly set

    functions->weapons_read_table(false, true, 0xFFFFFFFF);
}

static void InitGlobals()
{
    RSL2_GlobalState* globalState = GetGlobalState();

    //Set global pointers
    globalState->World = OffsetPtr<world*>(0x02B97490);
    globalState->MainCamera = OffsetPtr<rfg_camera*>(0x019E3B50);
    globalState->RfgMenusList = OffsetPtr<farray<ui_menu*, 8>*>(0x1266698);

    globalState->NumAirBombInfos = OffsetPtr<u32*>(0x01E28610);
    auto airBombInfosPtr = OffsetPtr<air_bomb_info**>(0x01E2860C);
    globalState->AirBombInfos.Init(*airBombInfosPtr, *globalState->NumAirBombInfos, *globalState->NumAirBombInfos, "AirBombInfos");

    globalState->NumWeaponInfos = OffsetPtr<u32*>(0x03481C94);
    auto weaponInfosPtr = OffsetPtr<weapon_info**>(0x03481C9C);
    globalState->WeaponInfos.Init(*weaponInfosPtr, *globalState->NumWeaponInfos, *globalState->NumWeaponInfos, "WeaponInfos");

    globalState->LOS_blocker_debug = OffsetPtr<bool*>(0x019ED4B7);
    globalState->Air_bomb_debug = OffsetPtr<bool*>(0x01E28601);
    globalState->Activity_register_damage_debug = OffsetPtr<bool*>(0x01E285C8);
    globalState->Tod_show_sun_path = OffsetPtr<bool*>(0x02132928);
    globalState->Player_hold_debug = OffsetPtr<bool*>(0x02C2E0D7);
    globalState->Salvage_debug = OffsetPtr<bool*>(0x02C2E225);
    globalState->Player_max_movement_speed_override = OffsetPtr<float*>(0x02C2E0B4);
    globalState->SpeedScale = OffsetPtr<float*>(0x0125BBD4);
    globalState->Use_packfiles = OffsetPtr<bool*>(0x01958C55);
    globalState->Xml_mempool = OffsetPtr<mempool_base**>(0x018E77DC);

    globalState->MouseGenericPollMouseVisibleAddress = globalState->ModuleBase + 0x001B88DC;
    globalState->CenterMouseCursorCallAddress = globalState->ModuleBase + 0x878D90;
}