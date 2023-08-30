#pragma once
#include "Functions.h"
#include "reconstructor/misc/GlobalState.h"

namespace rfg
{
    extern rfg::RfgFunctions Functions;
}

extern rfg::RfgFunctions* GetRfgFunctions();

//Helpers for showing/hiding the hud and fog
extern void ShowHud();
extern void HideHud();
extern void ToggleHud();
extern void ShowFog();
extern void HideFog();
extern void ToggleFog();