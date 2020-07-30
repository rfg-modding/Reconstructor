#pragma once
#include "rsl2/rfg/Game.h"

//Todo: Organize this properly
namespace rfg
{
    //game_state __cdecl gameseq_set_state(game_state state, bool uninterruptible)
    using F_GameseqGetState = game_state(__cdecl*)();
    extern F_GameseqGetState GameseqGetState;

    //void __cdecl gameseq_set_state(game_state state, bool uninterruptible)
    using F_GameseqSetState = void(__cdecl*)(game_state state, bool uninterruptible);
    extern F_GameseqSetState GameseqSetState;
}

//Todo: Organize this properly
template<typename T>
void RegisterFunction(T& Function, unsigned long RelativeAddress)
{
    static RSL2_GlobalState* state = GetGlobalState();
    Function = reinterpret_cast<T>(state->ModuleBase + RelativeAddress);
}