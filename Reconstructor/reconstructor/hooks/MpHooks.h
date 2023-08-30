#pragma once
#include "common/patching/FunHook.h"
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