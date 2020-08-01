#pragma once
#include "RFGR_Types/rfg/Game.h"
#include "RFGR_Types/rfg/Vector.h"
#include "RFGR_Types/rfg/Matrix.h"
#include "RFGR_Types/rfg/DrawData.h"
#include "RFGR_Types/rfg/Memory.h"
#include "RFGR_Types/rfg/Camera.h"

//Forward declarations
struct Human;

//Todo: Organize this properly
namespace rfg
{
    //game_state __cdecl gameseq_set_state(game_state state, bool uninterruptible)
    using F_GameseqGetState = game_state(__cdecl*)();
    extern F_GameseqGetState GameseqGetState;

    //void __cdecl gameseq_set_state(game_state state, bool uninterruptible)
    using F_GameseqSetState = void(__cdecl*)(game_state state, bool uninterruptible);
    extern F_GameseqSetState GameseqSetState;

    //int __cdecl memmgr_debug_render(split_memmgr *mgr, int sy) //0x003D25D0
    using F_memmgr_debug_render = bool(__cdecl*)(split_memmgr* mgr, int sy);
    extern F_memmgr_debug_render memmgr_debug_render;

    //int __cdecl memmgr_debug_render_tiny(split_memmgr *mgr, int sy)
    using F_memmgr_debug_render_tiny = int(__cdecl*)(split_memmgr* mgr, int sy);
    extern F_memmgr_debug_render_tiny memmgr_debug_render_tiny; //0x003E56A0

    //void __thiscall gr_state::gr_state(gr_state *this, gr_alpha_mode am, gr_clamp_mode cm, gr_zbuf_mode zm, gr_stencil_mode stm, unsigned int const_alpha, 
    //gr_cull_mode tclm, gr_tnl_clip_mode tcm, gr_tnl_light_mode tlm)
    using F_gr_state_constructor = void(__fastcall*)(gr_state* thisPtr, void* edx, gr_alpha_mode am, gr_clamp_mode cm, gr_zbuf_mode zm, gr_stencil_mode stm, unsigned int const_alpha,
                                                  gr_cull_mode tclm, gr_tnl_clip_mode tcm, gr_tnl_light_mode tlm);
    extern F_gr_state_constructor gr_state_constructor;

    //bool __cdecl gr_bbox_aligned(vector* mn, vector* mx, gr_state* state) //0015E3A0
    using F_gr_bbox_aligned = bool(__cdecl*)(vector* min, vector* max, gr_state* state);
    extern F_gr_bbox_aligned gr_bbox_aligned;

    //void __cdecl grd_3d_string(vector *pos, matrix *orient, float scale, const char *s, int fontnum, gr_state *state)
    using F_gr_3d_string = void(__cdecl*)(vector* pos, matrix* orient, float scale, const char* s, int fontnum, gr_state* state);
    extern F_gr_3d_string gr_3d_string;

    //void __cdecl gr_3d_line(vector *v0, vector *v1, gr_state *state)
    using F_gr_3d_line = void(__cdecl*)(vector* v0, vector* v1, gr_state* state);
    extern F_gr_3d_line gr_3d_line;

    //void __cdecl gr_string(int sx, int sy, const char *s, int fontnum, gr_state *state)
    using F_gr_string = void(__cdecl*)(int sx, int sy, const char* s, int fontnum, gr_state* state);
    extern F_gr_string gr_string; //0x0015EC80

    //void __cdecl gr_get_string_size(int *w, int *h, const char *text, int len, int fontnum)
    using F_get_string_size = void(__cdecl*)(int* w, int* h, const char* text, int len, int fontnum);
    extern F_get_string_size gr_get_string_size; //0x000B9DE0

    //int __cdecl gr_get_font_height(int fontnum)
    using F_get_font_height = int(__cdecl*)(int fontnum);
    extern F_get_font_height gr_get_font_height; //0x000B9E40

    //void __cdecl gr_set_color(int r, int g, int b, int a)
    using F_gr_set_color = void(__cdecl*)(int r, int g, int b, int a);
    extern F_gr_set_color gr_set_color; //0x001090C0
 
    //void __cdecl gr_line(int x1, int y1, int x2, int y2, gr_state *state, bool __formal)
    using F_gr_line = void(__cdecl*)(int x1, int y1, int x2, int y2, gr_state* state, bool __formal);
    extern F_gr_line gr_line; //0x00154DB0

    //bool __cdecl gr_sphere(vector *center, float radius, gr_state *state)
    using F_gr_sphere = bool(__cdecl*)(vector* center, float radius, gr_state* state);
    extern F_gr_sphere gr_sphere; //0x0015E160

    //bool __cdecl gr_bbox_oriented(vector *mn, vector *mx, vector *pos, matrix *orient, gr_state *state)
    using F_gr_bbox_oriented = bool(__cdecl*)(vector* mn, vector* mx, vector* pos, matrix* orient, gr_state* state);
    extern F_gr_bbox_oriented gr_bbox_oriented; //0x0015E520

    //void __cdecl gr_rect(int x1, int y1, int w, int h, gr_state *state)
    using F_gr_rect = void(__cdecl*)(int x1, int y1, int w, int h, gr_state* state);
    extern F_gr_rect gr_rect; //0x00154BC0

    //Todo: Setup all of these function pointers. Some gr_ functions not included as I don't have immediate use for them
    //void __cdecl gr_set_color(int r, int g, int b, int a) //0x001090C0
    //void __cdecl gr_set_color(float r, float g, float b, float a) //0x00109160
    //void __cdecl gr_set_alpha(int a) //0x001092B0
    //void __cdecl gr_start_instance(vector *pos, matrix *orient, matrix *scale) //0x00109320
    //void __cdecl gr_stop_instance() //0x001093E0
    //bool __cdecl gr_screen_coords_from_world_coords(vector *world_pos, float *screen_x, float *screen_y, float *screen_z, int allow_off_edges) //0x00109570

    //void __cdecl gr_rect_outline(int x, int y, int w, int h, int t, gr_state *state) //0x00154B00
    //void __cdecl gr_rect(int x1, int y1, int w, int h, gr_state *state) //0x00154BC0
    //void __cdecl gr_rect(float x, float y, float w, float h, gr_state *state) //0x00154C70
    //void __cdecl gr_line(float x1, float y1, float x2, float y2, gr_state *state, bool __formal) //0x00154D20
    //void __cdecl gr_line(int x1, int y1, int x2, int y2, gr_state *state, bool __formal) //0x00154DB0

    //void __cdecl gr_bitmap(int bitmap_id, int x, int y, gr_state *state) //0x00154E50
    //void __cdecl gr_bitmap_scaled(int bitmap_id, int x, int y, int w, int h, gr_state *state) //0x00154ED0
    //void __cdecl gr_bitmap_scaled(int bitmap_id, float x, float y, float w, float h, gr_state *state) //0x00154FB0
    //void __cdecl gr_bitmap_centered(int bitmap_id, int cx, int cy, gr_state *state) //0x00155080
    //void __cdecl gr_bitmap_ex(int bitmap_id, int x, int y, int w, int h, int sx, int sy, int sw, int sh, bool flip_x, bool flip_y, gr_state *state) //0x00155100
    //void __cdecl gr_bitmap_rotated_scale(int bitmap_id, float x, float y, float angle, float scalew, float scaleh, gr_state *state) //0x001551F0
    //void __cdecl gr_bitmap_rotated(int bitmap_id, float x, float y, float angle, float scale, gr_state *state) //0x001552D0

    //void __cdecl gr_circle(int x, int y, int radius, gr_state *state) //0x00155370
    //void __cdecl gr_string_scale(int sx, int sy, const char *s, float scale, int fontnum, gr_state *state) //0x00155400
    //void __cdecl gr_3d_bitmap(int bitmap_id, vector *pnt, matrix *orient, float radius, gr_state *state) //0x0015DF80

    //void __cdecl gr_3d_arrow(vector *v1, vector *v2, float arrowhead_pct, gr_state *state) //0x0015E0C0
    //bool __cdecl gr_sphere(vector *center, float radius, gr_state *state) //0x0015E160
    //bool __cdecl gr_sphere_solid(vector *center, float radius, int __formal, gr_state *state) //0x0015E1F0
    //bool __cdecl gr_cone(vector *cone_point_pos, matrix *orient, float radius, float height, gr_state *state) //0x0015E280
    //bool __cdecl gr_box(vector *center, matrix *orient, vector *size, gr_state *state) //0x0015E320
    //bool __cdecl gr_bbox_oriented(vector *mn, vector *mx, vector *pos, matrix *orient, gr_state *state) //0015E520
    //void __cdecl gr_bbox_solid(vector *bmin, vector *bmax, gr_state *state) //0x0015E710
    //void __cdecl gr_cylinder(vector *center, matrix *orient, float radius, float height, gr_state *state) //0x0015E790
    //void __cdecl gr_plane(vector *pt, matrix *orient, float w, float h, gr_state *state) //0x0015E830
    //void __cdecl gr_plane_solid(plane *pl, gr_state *state) //0x0015E8D0
    //void __cdecl gr_axis(vector *pos, matrix *orient, float scale, gr_state *state) //0x0015E9C0
    //bool __cdecl gr_circle(vector *center, float radius, vector axis, int __formal, gr_state *state) //0x0015EA50
    //void __cdecl gr_circle_xz(vector *center, float radius, gr_state *state) //0x0015EB20
    //void __cdecl gr_arc_xz(vector *center, vector *start, float angle, gr_state *state) //0x0015EBF0
    //void __cdecl gr_string(int sx, int sy, const char *s, int fontnum, gr_state *state) //0x0015EC80
    //void __cdecl gr_string(int sx, int sy, const wchar_t *s, int fontnum, gr_state *state) //0x0015ED30
    //void __cdecl gr_frustum_solid(vector *eye_pos, plane *planes, int nplanes, gr_state *state) //0x00161B50
    //void __cdecl gr_frustum(vector *eye_pos, plane *planes, int nplanes, gr_state *state) //0x00161BD0

    //void __cdecl hud_bm_res_independent(int bitmap_id, int cx, int cy, float scale, gr_state state) //0x00441CA0
    //void __cdecl hud_bm_pos_res_independent(int bitmap_id, int cx, int cy, float scale, gr_state state) //0x00441EF0

    //void __cdecl camera_push_mode(camera_mode m) //0x002C9240
    using F_camera_push_mode = void(__cdecl*)(camera_mode mode);
    extern F_camera_push_mode camera_push_mode;

    //void __cdecl camera_pop_mode(camera_mode m) //0x002C92D0
    using F_camera_pop_mode = void(__cdecl*)(camera_mode mode);
    extern F_camera_pop_mode camera_pop_mode;

    //void __cdecl human_teleport_unsafe(human *hp, vector pos, matrix orient) //0x0067C290
    using F_human_teleport_unsafe = void(__cdecl*)(Human* hp, vector pos, matrix orient);
    extern F_human_teleport_unsafe human_teleport_unsafe;

    //void __cdecl hud_hide(bool hide) //0x00441AE0
    using F_hud_hide = void(__cdecl*)(bool hide);
    extern F_hud_hide hud_hide;

    //void __cdecl game_render_set_fog_enabled(bool enabled) //0x003C2C50
    using F_game_render_set_fog_enabled = void(__cdecl*)(bool hide);
    extern F_game_render_set_fog_enabled game_render_set_fog_enabled;
}

//Todo: Organize this properly
template<typename T>
void RegisterFunction(T& Function, unsigned long RelativeAddress)
{
    static RSL2_GlobalState* state = GetGlobalState();
    Function = reinterpret_cast<T>(state->ModuleBase + RelativeAddress);
}

extern void InitFunctionPointers();

//Helpers for showing/hiding the hud and fog
extern void ShowHud();
extern void HideHud();
extern void ToggleHud();
extern void ShowFog();
extern void HideFog();
extern void ToggleFog();