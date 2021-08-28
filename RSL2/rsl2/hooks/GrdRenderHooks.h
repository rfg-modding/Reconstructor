#pragma once
#include "common/patching/FunHook.h"
#include "RFGR_Types/rfg/DrawData.h"
#include "rsl2/functions/FunctionsInternal.h"

//Hook all grd_ functions so they call their gr_ equivalents. Necessary since the grd_ functions are empty in public builds but gr_ functions aren't
//Lets us use the games built in debug rendering things which were broken because grd_ functions were removed.
//Note: Only really hooked the ones with xrefs in IDA to save time
FunHook<void(int sx, int sy, const char* s, gr_state* state)> grd_string_hook
{
    0x000B8DA0,
    [](int sx, int sy, const char* s, gr_state* state) -> void
    {
        int fontNum = 3;
        return rfg::Functions.gr_string(sx, sy, s, fontNum, state);
    }
};
FunHook<void(vector* pos, matrix* orient, float scale, const char* s, gr_state* state)> grd_3d_string_hook
{
    0x000B8DE0,
    [](vector* pos, matrix* orient, float scale, const char* s, gr_state* state) -> void
    {
        int fontNum = 3;
        return rfg::Functions.gr_3d_string(pos, orient, scale, s, fontNum, state);
    }
};
FunHook<void(int* w, int* h, const char* text, int len)> grd_get_string_size_hook
{
    0x000B8DF0,
    [](int* w, int* h, const char* text, int len) -> void
    {
        int fontNum = 3;
        return rfg::Functions.gr_get_string_size(w, h, text, len, fontNum);
    }
};
FunHook<int()> grd_get_font_height
{
    0x000B8E30,
    []() -> int
    {
        int fontNum = 3;
        return rfg::Functions.gr_get_font_height(fontNum);
    }
};

FunHook<void(int x, int y, int w, int h, gr_state* state)> grd_rect_hook
{
    0x000B8E40,
    [](int x, int y, int w, int h, gr_state* state) -> void
    {
        return rfg::Functions.gr_rect(x, y, w, h, state);
    }
};

FunHook<void(int r, int g, int b, int a)> grd_set_color_hook
{
    0x000B8E50,
    [](int r, int g, int b, int a) -> void
    {
        rfg::Functions.gr_set_color(r, g, b, a);
    }
};

FunHook<void(int x1, int y1, int x2, int y2, gr_state* state, bool clip)> grd_line_hook
{
    0x000B8EB0,
    [](int x1, int y1, int x2, int y2, gr_state* state, bool clip) -> void
    {
        return rfg::Functions.gr_line(x1, y1, x2, y2, state, clip);
    }
};
FunHook<bool(vector* center, float radius, gr_state* state)> grd_sphere_hook
{
    0x000B8EC0,
    [](vector* center, float radius, gr_state* state) -> bool
    {
        return rfg::Functions.gr_sphere(center, radius, state);
    }
};
FunHook<bool(vector* mn, vector* mx, gr_state* state)> grd_bbox_aligned_hook
{
    0x000B8F00,
    [](vector* mn, vector* mx, gr_state* state) -> bool
    {
        return rfg::Functions.gr_bbox_aligned(mn, mx, state);
    }
};
FunHook<bool(vector* mn, vector* mx, vector* pos, matrix* orient, gr_state* state)> grd_bbox_oriented_hook
{
    0x000B8F20,
    [](vector* mn, vector* mx, vector* pos, matrix* orient, gr_state* state) -> bool
    {
        return rfg::Functions.gr_bbox_oriented(mn, mx, pos, orient, state);
    }
};