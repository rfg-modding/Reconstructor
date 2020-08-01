#include "Functions.h"
#include "rsl2/misc/GlobalState.h"

namespace rfg
{
    F_GameseqGetState GameseqGetState = nullptr;
    F_GameseqSetState GameseqSetState = nullptr;
    
    F_memmgr_debug_render memmgr_debug_render;
    F_memmgr_debug_render_tiny memmgr_debug_render_tiny;

    F_gr_state_constructor gr_state_constructor = nullptr;
    F_gr_bbox_aligned gr_bbox_aligned = nullptr;
    F_gr_3d_string gr_3d_string = nullptr;
    F_gr_3d_line gr_3d_line = nullptr;
    F_gr_string gr_string = nullptr;
    F_get_string_size gr_get_string_size = nullptr;
    F_get_font_height gr_get_font_height = nullptr;
    F_gr_set_color gr_set_color = nullptr;
    F_gr_line gr_line = nullptr;
    F_gr_sphere gr_sphere = nullptr;
    F_gr_bbox_oriented gr_bbox_oriented = nullptr;
    F_gr_rect gr_rect = nullptr;
}

void InitFunctionPointers()
{
    RegisterFunction(rfg::GameseqGetState, 0x003BFC70);
    RegisterFunction(rfg::GameseqSetState, 0x003D8730);
    RegisterFunction(rfg::memmgr_debug_render, 0x003D25D0);
    RegisterFunction(rfg::memmgr_debug_render_tiny, 0x003E56A0);
    RegisterFunction(rfg::gr_state_constructor, 0x000B8C90);
    RegisterFunction(rfg::gr_bbox_aligned, 0x0015E3A0);
    RegisterFunction(rfg::gr_3d_string, 0x00155540);
    RegisterFunction(rfg::gr_3d_line, 0x0015E000);
    RegisterFunction(rfg::gr_string, 0x0015EC80);
    RegisterFunction(rfg::gr_get_string_size, 0x000B9DE0);
    RegisterFunction(rfg::gr_get_font_height, 0x000B9E40);
    RegisterFunction(rfg::gr_set_color, 0x001090C0);
    RegisterFunction(rfg::gr_line, 0x00154DB0);
    RegisterFunction(rfg::gr_sphere, 0x0015E160);
    RegisterFunction(rfg::gr_bbox_oriented, 0x0015E520);
    RegisterFunction(rfg::gr_rect, 0x00154BC0);
}