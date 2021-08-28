#pragma once
#include "common/patching/FunHook.h"
#include "RFGR_Types/rfg/Game.h"
#include "rsl2/functions/FunctionsInternal.h"
#include "rsl2/hooks/RenderHooks.h"
#include "rsl2/patching/Offset.h"
#include <magic_enum/include/magic_enum.hpp>
#include <imgui/imgui.h>

void SetCustomGameStates();
void CustomGameState_Enter(game_state state);
void CustomGameState_Exit(game_state state);
bool CustomGameState_ExitNotify(game_state state);
void CustomGameState_Process();
void CustomGameState_Handshake(void* renderer);
void CustomGameState_Render(void* renderer);

//void __cdecl gameseq_register_state(game_state state, state_info *callbacks) //0x003BFBD0
FunHook<void(game_state state, state_info* callbacks)> gameseq_register_state_hook
{
    0x0, //0x003BFBD0
    [](game_state state, state_info* callbacks) -> void
    {
        static u32 numRegisteredStates = 0;
        numRegisteredStates++;
        printf("gameseq_register_state_hook:: State: %s, Num registered states: %d\n", magic_enum::enum_name(state).data(), numRegisteredStates);
        gameseq_register_state_hook.CallTarget(state, callbacks);
    }
};

//void __cdecl gameseq_set_state(game_state state, bool uninterruptible) //0x003D8730
FunHook<void(game_state state, bool uninterruptible)> gameseq_set_state_hook
{
    0x0, //0x003D8730
    [](game_state state, bool uninterruptible) -> void
    {
        static u32 callCount = 0;
        callCount++;
        printf("gameseq_set_state_hook:: State: %s, uninterruptible: %s\n", magic_enum::enum_name(state).data(), uninterruptible ? "true" : "false");

        if (callCount == 2 && state == GS_MAINMENU)
        {
            //Set our custom state instead to get better control over the game
            SetCustomGameStates();
            gameseq_set_state_hook.CallTarget(GS_PS3_DLC, true); //Using GS_PS3_DLC because it's not used by re-mars-tered
            printf("Overriding first main menu state push with custom state\n");
        }
        else
        {
            gameseq_set_state_hook.CallTarget(state, uninterruptible);
        }
    }
};

//void __cdecl gameseq_push_state(game_state new_state, bool transparent, bool pause_beneath) //0x003D87E0
FunHook<void(game_state new_state, bool transparent, bool pause_beneath)> gameseq_push_state_hook
{
    0x0, //0x003D87E0
    [](game_state new_state, bool transparent, bool pause_beneath) -> void
    {
        printf("gameseq_push_state_hook:: NewState: %s, transparent: %s, pause_beneath: %s\n", magic_enum::enum_name(new_state).data(), transparent ? "true" : "false", pause_beneath ? "true" : "false");
        gameseq_push_state_hook.CallTarget(new_state, transparent, pause_beneath);
    }
};

void SetCustomGameStates()
{
    //Inject custom state to override the main menu the first time it's loaded
    state_info state;
    state.enter = &CustomGameState_Enter;
    state.exit = &CustomGameState_Exit;
    state.exit_notify = &CustomGameState_ExitNotify;
    state.process = &CustomGameState_Process;
    state.handshake = &CustomGameState_Handshake;
    state.render = &CustomGameState_Render;
    state.transparent = false;
    state.pause_beneath = false;
    state.user_flags = 0;
    state.hide_cursor = false;

    //void __cdecl gameseq_register_state(game_state state, state_info *callbacks) //0x003BFBD0

    //0x003BFBD0
    using F_gameseq_register_state = void(__cdecl*)(game_state state, state_info* callbacks);
    F_gameseq_register_state gameseq_register_state = nullptr;
    RegisterFunction(gameseq_register_state, 0x003BFBD0);
    gameseq_register_state(GS_PS3_DLC, &state);
}

void CustomGameState_Enter(game_state state)
{

}

void CustomGameState_Exit(game_state state)
{

}

bool CustomGameState_ExitNotify(game_state state)
{
    return false;
}

void CustomGameState_Process()
{

}

void CustomGameState_Handshake(void* renderer)
{

}

void CustomGameState_Render(void* renderer)
{
    static RSL2_GlobalState* globalState = GetGlobalState();
    static rfg::RfgFunctions* functions = GetRfgFunctions();
    if (!globalState || !globalState->ImGuiInitialized || globalState->Host->PerformingReload)
        return;

    if (!ImGui::Begin("Custom game state GUI"))
    {
        ImGui::End();
        return;
    }

    ImGui::Text("You're in a custom game state!");
    ImGui::End();
}