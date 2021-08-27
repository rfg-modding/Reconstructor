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

    //Primitive rendering helpers
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
    RegisterFunction(rfg::Functions.gr_set_color2, 0x00109160);
    RegisterFunction(rfg::Functions.gr_set_alpha, 0x001092B0);
    RegisterFunction(rfg::Functions.gr_start_instance, 0x00109320);
    RegisterFunction(rfg::Functions.gr_stop_instance, 0x001093E0);
    RegisterFunction(rfg::Functions.hud_bm_res_independent, 0x00441CA0);
    RegisterFunction(rfg::Functions.hud_bm_pos_res_independent, 0x00441EF0);
    RegisterFunction(rfg::Functions.gr_rect_outline, 0x00154B00);
    RegisterFunction(rfg::Functions.gr_rect2, 0x00154C70);
    RegisterFunction(rfg::Functions.gr_line2, 0x00154D20);
    RegisterFunction(rfg::Functions.gr_circle, 0x00155370);
    RegisterFunction(rfg::Functions.gr_string_scale, 0x00155400);
    RegisterFunction(rfg::Functions.gr_bbox_solid, 0x0015E710);
    RegisterFunction(rfg::Functions.gr_cylinder, 0x0015E790);
    RegisterFunction(rfg::Functions.gr_plane, 0x0015E830);
    RegisterFunction(rfg::Functions.gr_plane_solid, 0x0015E8D0);
    RegisterFunction(rfg::Functions.gr_axis, 0x0015E9C0);
    RegisterFunction(rfg::Functions.gr_circle_xz, 0x0015EB20);
    RegisterFunction(rfg::Functions.gr_arc_xz, 0x0015EBF0);
    RegisterFunction(rfg::Functions.gr_string2, 0x0015ED30);
    RegisterFunction(rfg::Functions.gr_frustum_solid, 0x00161B50);
    RegisterFunction(rfg::Functions.gr_frustum, 0x00161BD0);
    RegisterFunction(rfg::Functions.gr_screen_coords_from_world_coords, 0x00109570);
    RegisterFunction(rfg::Functions.gr_sphere_solid, 0x0015E1F0);
    RegisterFunction(rfg::Functions.gr_cone, 0x0015E280);
    RegisterFunction(rfg::Functions.gr_box, 0x0015E320);
    RegisterFunction(rfg::Functions.gr_3d_circle, 0x0015EA50);
    RegisterFunction(rfg::Functions.gr_3d_bitmap, 0x0015DF80);
    //RegisterFunction(rfg::Functions.gr_3d_arrow, 0x0015E0C0);
    //RegisterFunction(rfg::Functions.gr_bitmap, 0x00154E50);
    //RegisterFunction(rfg::Functions.gr_bitmap_scaled, 0x00154ED0);
    //RegisterFunction(rfg::Functions.gr_bitmap_scaled2, 0x00154FB0);
    //RegisterFunction(rfg::Functions.gr_bitmap_centered, 0x00155080);
    //RegisterFunction(rfg::Functions.gr_bitmap_ex, 0x00155100);
    //RegisterFunction(rfg::Functions.gr_bitmap_rotated_scale, 0x001551F0);
    //RegisterFunction(rfg::Functions.gr_bitmap_rotated, 0x001552D0);

    RegisterFunction(rfg::Functions.camera_push_mode, 0x002C9240);
    RegisterFunction(rfg::Functions.camera_pop_mode, 0x002C92D0);
    RegisterFunction(rfg::Functions.human_teleport_unsafe, 0x0067C290);
    RegisterFunction(rfg::Functions.hud_hide, 0x00441AE0);
    RegisterFunction(rfg::Functions.game_render_set_fog_enabled, 0x003C2C50);

    RegisterFunction(rfg::Functions.audiolib_cue_play, 0x0009F100);
    RegisterFunction(rfg::Functions.audiolib_cue_get_id, 0x0008DC20);
    RegisterFunction(rfg::Functions.audiolib_play_fade_out, 0x00094AD0);

    RegisterFunction(rfg::Functions.air_bomb_start, 0x0035C770);
    RegisterFunction(rfg::Functions.air_bomb_stop, 0x0035C810);
    RegisterFunction(rfg::Functions.air_bomb_stop_all, 0x0035CA20);

    RegisterFunction(rfg::Functions.cf_open, 0x001B5BB0);
    RegisterFunction(rfg::Functions.scripting_system_write_help_file, 0x00737B50);

    RegisterFunction(rfg::Functions.los_blocker_render_debug, 0x002ED680);
    RegisterFunction(rfg::Functions.air_bomb_render_debug, 0x00372970);
    RegisterFunction(rfg::Functions.activity_register_damage_render_debug, 0x00385860);
    RegisterFunction(rfg::Functions.game_time_of_day_render_debug, 0x003E97B0);
    RegisterFunction(rfg::Functions.player_hold_render_debug, 0x00611590);
    RegisterFunction(rfg::Functions.salvage_render_debug, 0x00672100);
    RegisterFunction(rfg::Functions.rfg_rbb_render_debug, 0x0031FC40);

    RegisterFunction(rfg::Functions.game_enable_standard_access, 0x001D2650);
    RegisterFunction(rfg::Functions.xml_parse_from_string, 0x001BF870);
    RegisterFunction(rfg::Functions.weapons_read_table, 0x007DF4D0);

    RegisterFunction(rfg::Functions.ui_create_static_mesh, 0x004C48C0);
    RegisterFunction(rfg::Functions.static_mesh_find, 0x004094A0);

    RegisterFunction(rfg::Functions.stream_grid_set_stream_radius, 0x0051AD30);
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