#pragma once
#include "common/patching/FunHook.h"
#include "common/patching/MemUtils.h"
#include "common/Typedefs.h"

//Todo: Implement type
struct sender;

//This is the handler for the remote console command packet built into RFGR.
//In theory this would allow players to run commands from the in game console on other players games.
//In practice that's not a real risk since the built in console can't do much except hide the UI or talk in MP chat.
//It's disabled here anyway just in case the built in console is expanded in the future.
FunHook<void(char* data, sender* from)> multi_game_process_remote_console_command_hook
{
    0x0054E7C0,
    [](char* data, sender* from)
    {
        return;
    }
};

void ApplyMultiplayerUIPatches()
{
    WriteMem<int8_t>(0x16624B0, 0); // disable the matchmaking item in multiplayer menu
    WriteMem<int8_t>(0x16624F0, 0); // disable the spectator item in multiplayer menu

    // set first multiplayer option (matchmaking) to find games instead, the user could
    // still access this if they go to multiplayer menu, not move mouse and press enter
    WriteMem<int8_t>(0x8AD61D, GS_MULTIPLAYER_LIVE_FIND_SERVERS);
}

























