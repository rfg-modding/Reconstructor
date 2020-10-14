#pragma once
#include "rsl2/misc/GlobalState.h"

std::string CharArrayToString(char* Array, int Size)
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

std::string CharArrayToString(const char* Array, int Size)
{
    return CharArrayToString(const_cast<char*>(Array), Size);
}

//Attempts to disable the main menu buttons for disabled features such as MP, LAN, and Wrecking Crew.
void TryHideInvalidMainMenuOptions()
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