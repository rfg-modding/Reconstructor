#include "FunctionsInternal.h"

namespace rfg
{
    rfg::RfgFunctions Functions;
}

rfg::RfgFunctions* GetRfgFunctions()
{
    return &rfg::Functions;
}

bool HudVisible = true;
void ShowHud()
{
    rfg::Functions.hud_hide(false);
    HudVisible = true;
}
void HideHud()
{
    rfg::Functions.hud_hide(true);
    HudVisible = false;
}
void ToggleHud()
{
    if (HudVisible)
        HideHud();
    else
        ShowHud();
}

bool FogVisible = true;
void ShowFog()
{
    rfg::Functions.game_render_set_fog_enabled(true);
    FogVisible = true;
}
void HideFog()
{
    rfg::Functions.game_render_set_fog_enabled(false);
    FogVisible = false;
}
void ToggleFog()
{
    if (FogVisible)
        HideFog();
    else
        ShowFog();
}