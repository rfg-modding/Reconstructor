#include "FunctionsInternal.h"

namespace rfg
{
    rfg::RfgFunctions Functions;
}

void InitFunctionPointers()
{
    RegisterFunction(rfg::Functions.GameseqGetState, 0x003BFC70);
    RegisterFunction(rfg::Functions.GameseqSetState, 0x003D8730);

    RegisterFunction(rfg::Functions.memmgr_debug_render, 0x003D25D0);
    RegisterFunction(rfg::Functions.memmgr_debug_render_tiny, 0x003E56A0);

    RegisterFunction(rfg::Functions.gr_state_constructor, 0x000B8C90);
    RegisterFunction(rfg::Functions.gr_bbox_aligned, 0x0015E3A0);
    RegisterFunction(rfg::Functions.gr_3d_string, 0x00155540);
    RegisterFunction(rfg::Functions.gr_3d_line, 0x0015E000);
    RegisterFunction(rfg::Functions.gr_string, 0x0015EC80);
    RegisterFunction(rfg::Functions.gr_get_string_size, 0x000B9DE0);
    RegisterFunction(rfg::Functions.gr_get_font_height, 0x000B9E40);
    RegisterFunction(rfg::Functions.gr_set_color, 0x001090C0);
    RegisterFunction(rfg::Functions.gr_line, 0x00154DB0);
    RegisterFunction(rfg::Functions.gr_sphere, 0x0015E160);
    RegisterFunction(rfg::Functions.gr_bbox_oriented, 0x0015E520);
    RegisterFunction(rfg::Functions.gr_rect, 0x00154BC0);

    RegisterFunction(rfg::Functions.camera_push_mode, 0x002C9240);
    RegisterFunction(rfg::Functions.camera_pop_mode, 0x002C92D0);
    RegisterFunction(rfg::Functions.human_teleport_unsafe, 0x0067C290);
    RegisterFunction(rfg::Functions.hud_hide, 0x00441AE0);
    RegisterFunction(rfg::Functions.game_render_set_fog_enabled, 0x003C2C50);
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