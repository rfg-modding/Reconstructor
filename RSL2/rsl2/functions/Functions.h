#pragma once
#include "rsl2/rfg/Game.h"
#include "rsl2/rfg/Vector.h"
#include "rsl2/rfg/Matrix.h"
#include "rsl2/rfg/DrawData.h"

//Todo: Organize this properly
namespace rfg
{
    //game_state __cdecl gameseq_set_state(game_state state, bool uninterruptible)
    using F_GameseqGetState = game_state(__cdecl*)();
    extern F_GameseqGetState GameseqGetState;

    //void __cdecl gameseq_set_state(game_state state, bool uninterruptible)
    using F_GameseqSetState = void(__cdecl*)(game_state state, bool uninterruptible);
    extern F_GameseqSetState GameseqSetState;

    //void __thiscall gr_state::gr_state(gr_state *this, gr_alpha_mode am, gr_clamp_mode cm, gr_zbuf_mode zm, gr_stencil_mode stm, unsigned int const_alpha, 
    //gr_cull_mode tclm, gr_tnl_clip_mode tcm, gr_tnl_light_mode tlm)
    using F_gr_state_constructor = bool(__fastcall*)(gr_state* thisPtr, void* edx, gr_alpha_mode am, gr_clamp_mode cm, gr_zbuf_mode zm, gr_stencil_mode stm, unsigned int const_alpha,
                                                  gr_cull_mode tclm, gr_tnl_clip_mode tcm, gr_tnl_light_mode tlm);
    extern F_gr_state_constructor gr_state_constructor;

    //bool __cdecl gr_bbox_aligned(vector* mn, vector* mx, gr_state* state) //0015E3A0
    using F_gr_bbox_aligned = bool(__cdecl*)(vector* min, vector* max, gr_state* state);
    extern F_gr_bbox_aligned gr_bbox_aligned;

    //void __cdecl grd_3d_string(vector *pos, matrix *orient, float scale, const char *s, int fontnum, gr_state *state)
    using F_gr_3d_string = bool(__cdecl*)(vector* pos, matrix* orient, float scale, const char* s, int fontnum, gr_state* state);
    extern F_gr_3d_string gr_3d_string;

    //void __cdecl gr_3d_line(vector *v0, vector *v1, gr_state *state)
    using F_gr_3d_line = bool(__cdecl*)(vector* v0, vector* v1, gr_state* state);
    extern F_gr_3d_line gr_3d_line;
}

//Todo: Organize this properly
template<typename T>
void RegisterFunction(T& Function, unsigned long RelativeAddress)
{
    static RSL2_GlobalState* state = GetGlobalState();
    Function = reinterpret_cast<T>(state->ModuleBase + RelativeAddress);
}