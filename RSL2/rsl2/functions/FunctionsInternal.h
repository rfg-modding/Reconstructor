#pragma once
#include "Functions.h"
#include "rsl2/misc/GlobalState.h"

/*
    This file contains helpers for setting up and using the function pointers in Functions.h 
    which we don't want to expose to other plugins. We only want to expose the function types
    to plugins, which are all that's in Functions.h
*/

namespace rfg
{
    extern rfg::RfgFunctions Functions;
}

//Todo: Organize this properly
template<typename T>
void RegisterFunction(T& Function, unsigned long RelativeAddress)
{
    static RSL2_GlobalState* state = GetGlobalState();
    Function = reinterpret_cast<T>(state->ModuleBase + RelativeAddress);
}

extern void InitFunctionPointers();
extern rfg::RfgFunctions* GetRfgFunctions();

//Helpers for showing/hiding the hud and fog
extern void ShowHud();
extern void HideHud();
extern void ToggleHud();
extern void ShowFog();
extern void HideFog();
extern void ToggleFog();