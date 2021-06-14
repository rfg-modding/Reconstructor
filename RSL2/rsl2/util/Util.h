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

    //Patch weapon objects. weapons_read_table() doesn't update some values. E.g. meshes
    for (u32 i = 0; i < globalState->World->all_objects.Size(); i++)
    {
        object* obj = globalState->World->all_objects[i];
        if (obj->obj_type != OT_ITEM || obj->sub_type != OT_SUB_ITEM_WEAPON)
            continue;

        //If <Item_3d> changed update the weapons mesh
        weapon* weap = (weapon*)obj;
        item::resource_dependent_data* itemRdd = weap->rdd.GetData();
        if (string(weap->w_info->weapon_obj_item_info->name) != string(weap->info->name) && itemRdd)
        {
            //Update item_info ptr
            weap->info = weap->w_info->weapon_obj_item_info;

            //Set new static meshes
            itemRdd->pickup_smesh = functions->static_mesh_find(weap->info->pickup_smesh_name, weap->info->srid);
            itemRdd->smesh = functions->static_mesh_find(weap->info->cmesh_name, weap->info->high_res_srid);

            //Release old mesh instances
            if (itemRdd->pickup_smesh_instance_p)
            {
                itemRdd->pickup_smesh_instance_p->release();
                itemRdd->pickup_smesh_instance_p = nullptr;
            }
            if (itemRdd->smesh_instance_p)
            {
                itemRdd->smesh_instance_p->release();
                itemRdd->smesh_instance_p = nullptr;
            }

            //Create new mesh instance using new static meshes
            if (itemRdd->pickup_smesh)
                itemRdd->pickup_smesh_instance_p = functions->ui_create_static_mesh(itemRdd->pickup_smesh);
            if (itemRdd->smesh)
                itemRdd->smesh_instance_p = functions->ui_create_static_mesh(itemRdd->smesh);
        }
    }
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