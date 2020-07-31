#include "Functions.h"
#include "rsl2/misc/GlobalState.h"

namespace rfg
{
    F_GameseqGetState GameseqGetState = nullptr;
    F_GameseqSetState GameseqSetState = nullptr;
    
    F_gr_state_constructor gr_state_constructor = nullptr;
    F_gr_bbox_aligned gr_bbox_aligned = nullptr;
    F_gr_3d_string gr_3d_string = nullptr;
    F_gr_3d_line gr_3d_line = nullptr;
}