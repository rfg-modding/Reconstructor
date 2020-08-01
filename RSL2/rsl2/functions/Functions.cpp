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