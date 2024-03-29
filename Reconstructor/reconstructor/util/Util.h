#pragma once
#include "reconstructor/misc/GlobalState.h"
#include "reconstructor/functions/FunctionsInternal.h"
#include "common/patching/Offset.h"
#include "RFGR_Types/rfg/World.h"
#include "RFGR_Types/rfg/Weapon.h"
#include "common/string/String.h"

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
    Reconstructor_GlobalState* globalState = GetGlobalState();
    if (!globalState->RfgMenusList || !(*globalState->RfgMenusList)[0])
        return;

    ui_menu& Menu = *(*globalState->RfgMenusList)[0];
    if (Menu.m_num_items == 9)
    {
        if (Menu.m_items[0])
        {
            if (CharArrayToString(Menu.m_items[0]->m_caption, 128) == "MENU_NEW_GAME") //The main menu doesn't have a title, so we have to look for it's buttons
            {
                ui_menu_item_base& wreckingCrewButton = *Menu.m_items[2];
                wreckingCrewButton.m_enabled = false;
                wreckingCrewButton.m_visible = false;

                ui_menu_item_base& multiplayerButton = *Menu.m_items[3];
                multiplayerButton.m_enabled = false;
                multiplayerButton.m_visible = false;

                ui_menu_item_base& lanButton = *Menu.m_items[5];
                lanButton.m_enabled = false;
                lanButton.m_visible = false;
            }
        }
    }
}

static void ReloadWeaponsXtbl();
static void ReloadXtbls()
{
    Reconstructor_GlobalState* globalState = GetGlobalState();
    rfg::RfgFunctions* functions = GetRfgFunctions();
    if (!globalState || !functions || !globalState->WeaponInfos.GetRawPointer())
        return;

    ReloadWeaponsXtbl();
}

//Reloads weapons.xtbl and aim_drift.xtbl and patches memory so that the changes are applied
static void ReloadWeaponsXtbl()
{
    Reconstructor_GlobalState* globalState = GetGlobalState();
    rfg::RfgFunctions* functions = GetRfgFunctions();

    //Clear weapon info states. weapons_read_table() doesn't reset them all.
    auto& weaponInfos = globalState->WeaponInfos;
    for (u32 i = 0; i < weaponInfos.Size(); i++)
    {
        weapon_info& info = weaponInfos[i];
        //Reset flags
        memset(&info.flags, 0, sizeof(weapon_info_flags));

        //Reset explosions
        info.m_explosion_info = nullptr;
        info.m_ai_explosion_info = nullptr;

        //Reset effects inside <Visuals> block
        info.muzzle_flash_effect = 0xFFFFFFFF;
        info.muzzle_smoke_effect = 0xFFFFFFFF;
        info.special_hit_effect = 0xFFFFFFFF;
        info.special_effect = 0xFFFFFFFF;
        info.secondary_special_effect = 0xFFFFFFFF;
        info.overheated_effect = 0xFFFFFFFF;
        info.tracer_effect = 0xFFFFFFFF;
    }

    //Reload weapons.xtbl and aim_drift.xtbl
    functions->weapons_read_table(false, true, 0xFFFFFFFF);

    //Patch weapon values not handled by weapons_read_table()
    for (u32 i = 0; i < globalState->World->all_objects.Size(); i++)
    {
        object* obj = globalState->World->all_objects[i];
        if (obj->obj_type != OT_ITEM || obj->sub_type != OT_SUB_ITEM_WEAPON)
            continue;

        weapon* weap = (weapon*)obj;

        //Refill magizine and ammo reserve
        weap->rounds_in_magazine = weap->w_info->magazine_size;
        weap->rounds_in_reserve = weap->w_info->max_rounds - weap->rounds_in_magazine;
    }

    //Todo: Todo: Fix these issues with weapons.xtbl hot reload
    //  - Changing <reticule_name>, <icon_name>, and <Item_3d> doesn't work
}

static void InitGlobals()
{
    Reconstructor_GlobalState* globalState = GetGlobalState();

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