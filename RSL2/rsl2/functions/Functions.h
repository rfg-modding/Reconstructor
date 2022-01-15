#pragma once
#include "RFGR_Types/rfg/Game.h"
#include "RFGR_Types/rfg/Vector.h"
#include "RFGR_Types/rfg/Plane.h"
#include "RFGR_Types/rfg/Matrix.h"
#include "RFGR_Types/rfg/DrawData.h"
#include "RFGR_Types/rfg/Memory.h"
#include "RFGR_Types/rfg/Camera.h"
#include "RFGR_Types/rfg/Audiolib.h"
#include "RFGR_Types/rfg/Weapon.h"
#include "RFGR_Types/rfg/stream2/cfile.h"
#include "common/patching/FunctionPointer.h"

//Forward declarations
struct human;
struct rfg_rbb_node;
struct xml_element;

namespace rfg
{
    class RfgFunctions
    {
    public:
        FunctionPointer<game_state __cdecl()> GameseqGetState = { 0x003BFC70 };
        FunctionPointer<void __cdecl(game_state state, bool uninterruptible)> GameseqSetState = { 0x003D8730 };
        FunctionPointer<void __cdecl(split_memmgr* mgr, int sy)> memmgr_debug_render = { 0x003D25D0 };
        FunctionPointer<void __cdecl(split_memmgr* mgr, int sy)> memmgr_debug_render_tiny = { 0x003E56A0 };

        FunctionPointer<void __fastcall(gr_state* thisPtr, void* edx, gr_alpha_mode am, gr_clamp_mode cm, gr_zbuf_mode zm, gr_stencil_mode stm, unsigned int const_alpha,
                                        gr_cull_mode tclm, gr_tnl_clip_mode tcm, gr_tnl_light_mode tlm)> gr_state_constructor = { 0x000B8C90 };
        FunctionPointer<bool __cdecl(vector* min, vector* max, gr_state* state)> gr_bbox_aligned = { 0x0015E3A0 };
        FunctionPointer<void __cdecl(vector* pos, matrix* orient, float scale, const char* s, int fontnum, gr_state* state)> gr_3d_string = { 0x00155540 };
        FunctionPointer<void __cdecl(vector* v0, vector* v1, gr_state* state)> gr_3d_line = { 0x0015E000 };
        FunctionPointer<void __cdecl(int sx, int sy, const char* s, int fontnum, gr_state* state)> gr_string = { 0x0015EC80 };
        FunctionPointer<void __cdecl(int* w, int* h, const char* text, int len, int fontnum)> gr_get_string_size = { 0x000B9DE0 };
        FunctionPointer<int __cdecl(int fontnum)> gr_get_font_height = { 0x000B9E40 };
        FunctionPointer<void __cdecl(int r, int g, int b, int a)> gr_set_color = { 0x001090C0 };
        FunctionPointer<void __cdecl(int x1, int y1, int x2, int y2, gr_state* state, bool __formal)> gr_line = { 0x00154DB0 };
        FunctionPointer<bool __cdecl(vector* center, float radius, gr_state* state)> gr_sphere = { 0x0015E160 };
        FunctionPointer<bool __cdecl(vector* mn, vector* mx, vector* pos, matrix* orient, gr_state* state)> gr_bbox_oriented = { 0x0015E520 };
        FunctionPointer<void __cdecl(int x1, int y1, int w, int h, gr_state* state)> gr_rect = { 0x00154BC0 };
        FunctionPointer<void __cdecl(float r, float g, float b, float a)> gr_set_color2 = { 0x00109160 };
        FunctionPointer<void __cdecl(int a)> gr_set_alpha = { 0x001092B0 };
        FunctionPointer<void __cdecl(vector* pos, matrix* orient, matrix* scale)> gr_start_instance = { 0x00109320 };
        FunctionPointer<void __cdecl()> gr_stop_instance = { 0x001093E0 };
        FunctionPointer<void __cdecl(int bitmap_id, int cx, int cy, float scale, gr_state state)> hud_bm_res_independent = { 0x00441CA0 };
        FunctionPointer<void __cdecl(int bitmap_id, int cx, int cy, float scale, gr_state state)> hud_bm_pos_res_independent = { 0x00441EF0 };
        FunctionPointer<void __cdecl(int x, int y, int w, int h, int t, gr_state* state)> gr_rect_outline = { 0x00154B00 };
        FunctionPointer<void __cdecl(float x, float y, float w, float h, gr_state* state)> gr_rect2 = { 0x00154C70 };
        FunctionPointer<void __cdecl(float x1, float y1, float x2, float y2, gr_state* state, bool __formal)> gr_line2 = { 0x00154D20 };
        FunctionPointer<void __cdecl(int x, int y, int radius, gr_state* state)> gr_circle = { 0x00155370 };
        FunctionPointer<void __cdecl(int sx, int sy, const char* s, float scale, int fontnum, gr_state* state)> gr_string_scale = { 0x00155400 };
        FunctionPointer<void __cdecl(vector* bmin, vector* bmax, gr_state* state)> gr_bbox_solid = { 0x0015E710 };
        FunctionPointer<void __cdecl(vector* center, matrix* orient, float radius, float height, gr_state* state)> gr_cylinder = { 0x0015E790 };
        FunctionPointer<void __cdecl(vector* pt, matrix* orient, float w, float h, gr_state* state)> gr_plane = { 0x0015E830 };
        FunctionPointer<void __cdecl(plane* pl, gr_state* state)> gr_plane_solid = { 0x0015E8D0 };
        FunctionPointer<void __cdecl(vector* pos, matrix* orient, float scale, gr_state* state)> gr_axis = { 0x0015E9C0 };
        FunctionPointer<void __cdecl(vector* center, float radius, gr_state* state)> gr_circle_xz = { 0x0015EB20 };
        FunctionPointer<void __cdecl(vector* center, vector* start, float angle, gr_state* state)> gr_arc_xz = { 0x0015EBF0 };
        FunctionPointer<void __cdecl(int sx, int sy, const wchar_t* s, int fontnum, gr_state* state)> gr_string2 = { 0x0015ED30 };
        FunctionPointer<void __cdecl(vector* eye_pos, plane* planes, int nplanes, gr_state* state)> gr_frustum_solid = { 0x00161B50 };
        FunctionPointer<void __cdecl(vector* eye_pos, plane* planes, int nplanes, gr_state* state)> gr_frustum = { 0x00161BD0 };
        FunctionPointer<bool __cdecl(vector* world_pos, float* screen_x, float* screen_y, float* screen_z, int allow_off_edges)> gr_screen_coords_from_world_coords = { 0x00109570 };
        FunctionPointer<bool __cdecl(vector* center, float radius, int __formal, gr_state* state)> gr_sphere_solid = { 0x0015E1F0 };
        FunctionPointer<bool __cdecl(vector* cone_point_pos, matrix* orient, float radius, float height, gr_state* state)> gr_cone = { 0x0015E280 };
        FunctionPointer<bool __cdecl(vector* center, matrix* orient, vector* size, gr_state* state)> gr_box = { 0x0015E320 };
        FunctionPointer<bool __cdecl(vector* center, float radius, vector axis, int __formal, gr_state* state)> gr_3d_circle = { 0x0015EA50 };
        FunctionPointer<void __cdecl(int bitmap_id, vector* pnt, matrix* orient, float radius, gr_state* state)> gr_3d_bitmap = { 0x0015DF80 };

        //Todo: Fix these functions
        //Note: These primitive render functions either don't work or crash the game
        //FunctionPointer<void __cdecl(vector* v1, vector* v2, float arrowhead_pct, gr_state* state)> gr_3d_arrow = { 0x0015E0C0 };
        //FunctionPointer<void __cdecl(int bitmap_id, int x, int y, gr_state* state)> gr_bitmap = { 0x00154E50 };
        //FunctionPointer<void __cdecl(int bitmap_id, int x, int y, int w, int h, gr_state* state)> gr_bitmap_scaled = { 0x00154ED0 };
        //FunctionPointer<void __cdecl(int bitmap_id, float x, float y, float w, float h, gr_state* state)> gr_bitmap_scaled2 = { 0x00154FB0 };
        //FunctionPointer<void __cdecl(int bitmap_id, int cx, int cy, gr_state* state)> gr_bitmap_centered = { 0x00155080 };
        //FunctionPointer<void __cdecl(int bitmap_id, int x, int y, int w, int h, int sx, int sy, int sw, int sh, bool flip_x, bool flip_y, gr_state* state)> gr_bitmap_ex = { 0x00155100 };
        //FunctionPointer<void __cdecl(int bitmap_id, float x, float y, float angle, float scalew, float scaleh, gr_state* state)> gr_bitmap_rotated_scale = { 0x001551F0 };
        //FunctionPointer<void __cdecl(int bitmap_id, float x, float y, float angle, float scale, gr_state* state)> gr_bitmap_rotated = { 0x001552D0 };

        FunctionPointer<void __cdecl(camera_mode mode)> camera_push_mode = { 0x002C9240 };
        FunctionPointer<void __cdecl(camera_mode mode)> camera_pop_mode = { 0x002C92D0 };
        FunctionPointer<void __cdecl(human* hp, vector pos, matrix orient)> human_teleport_unsafe = { 0x0067C290 };
        FunctionPointer<void __cdecl(bool hide)> hud_hide = { 0x00441AE0 };
        FunctionPointer<void __cdecl(bool hide)> game_render_set_fog_enabled = { 0x003C2C50 };
        FunctionPointer<int __cdecl(int cue_id, audiolib_params* passed_params, audiolib_result* error)> audiolib_cue_play = { 0x0009F100 };
        FunctionPointer<audiolib_result __cdecl(const char* cue_name, int* caller_cue_id)> audiolib_cue_get_id = { 0x0008DC20 };
        FunctionPointer<audiolib_result __cdecl(int play_id, const int fade_time_ms)> audiolib_play_fade_out = { 0x00094AD0 };
        FunctionPointer<int __cdecl(air_bomb_info* ab_info, unsigned int target_handle, unsigned int origin_handle, bool following)> air_bomb_start = { 0x0035C770 };
        FunctionPointer<void __cdecl(air_bomb_handle handle)> air_bomb_stop = { 0x0035C810 };
        FunctionPointer<void __cdecl()> air_bomb_stop_all = { 0x0035CA20 };
        FunctionPointer<cfile* __cdecl(char* buf, unsigned int buf_size, const char* open_mode, vlib_platform disk_platform)> cf_open = { 0x001B5BB0 };
        FunctionPointer<void __cdecl(const char* filename)> scripting_system_write_help_file = { 0x00737B50 };

        /*
            Debug render functions: Only some work. Others either dont work at all or crash the game
        */
        //Doesn't appear to work. Shows values in bottom left corner of screen that are always 0
        FunctionPointer<void __cdecl()> los_blocker_render_debug = { 0x002ED680 };
        //No visible effect
        FunctionPointer<void __cdecl()> air_bomb_render_debug = { 0x00372970 };
        //No visible effect
        FunctionPointer<void __cdecl()> activity_register_damage_render_debug = { 0x00385860 };
        //Crashes the game
        FunctionPointer<void __cdecl()> game_time_of_day_render_debug = { 0x003E97B0 };
        //No visible effect
        FunctionPointer<void __cdecl()> player_hold_render_debug = { 0x00611590 };
        //Works. Shows a 3d string above each object in the world that you can collect salvage from
        FunctionPointer<void __cdecl()> salvage_render_debug = { 0x00672100 };
        //Sorta works but unusable. It seems to draw bounding boxes for some kind of collision tree buildings have.
        //The problem is that they're rendered at the center of the world rather than relative to the object position and it's very laggy.
        //There's a re-implementation in guiBase/debugDraw/debugDraw.cpp called DrawRfgRbbNode(). It fixes the position problem but is still laggy.
        //At some point a tool could be added that would let you select a single building and display the bounding boxes for that to avoid perf issues.
        //For the moment it's disabled though.
        FunctionPointer<void __cdecl(rfg_rbb_node* tree)> rfg_rbb_render_debug = { 0x0031FC40 };

        FunctionPointer<void __cdecl()> game_enable_standard_access = { 0x001D2650 };
        FunctionPointer<xml_element*(char* buf, mempool_base* dest, const char* filename_orig)> xml_parse_from_string = { 0x001BF870 };
        FunctionPointer<void __cdecl(bool mp_override, bool refresh, int dlc_id)> weapons_read_table = { 0x007DF4D0 };

        FunctionPointer<rl_mesh_instance* __cdecl(static_mesh* base_mesh)> ui_create_static_mesh = { 0x004C48C0 };
        FunctionPointer<static_mesh* __cdecl(const char* filename, unsigned int srid)> static_mesh_find = { 0x004094A0 };
    };
}