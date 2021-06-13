#pragma once
#include "rsl2/misc/GlobalState.h"
#include "rsl2/functions/FunctionsInternal.h"

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
    if (!globalState || !functions)
        return;

    functions->weapons_read_table(false, true, 0xFFFFFFFF);
    /*Todo: Fix values that weapons_read_table doesn't update properly. Known values so far:
        - Weapon flags. If you remove a flag in the xtbl and reload it it's not removed from weapons in game
        - Explosions. Removing an explosion in the xtbl doesn't remove it from weapons in game.
        - Todo: Check other values and see if others aren't being updated properly
    */ 
}