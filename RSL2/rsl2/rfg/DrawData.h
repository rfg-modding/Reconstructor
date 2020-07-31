#pragma once

/* 612 */
enum gr_alpha_mode
{
    ALPHA_BLEND_NONE = 0x0,
    ALPHA_BLEND_ADDITIVE = 0x1,
    ALPHA_BLEND_ALPHA_ADDITIVE = 0x2,
    ALPHA_BLEND_ALPHA_ADDITIVE_CONST = 0x3,
    ALPHA_BLEND_ALPHA_ADDITIVE_SCREEN = 0x4,
    ALPHA_BLEND_FILTER = 0x5,
    ALPHA_BLEND_FILTER_SCREEN = 0x6,
    ALPHA_BLEND_FILTER_SCREEN_INVERSE = 0x7,
    ALPHA_BLEND_FILTER_CONST = 0x8,
    ALPHA_BLEND_SUBTRACTIVE = 0x9,
    ALPHA_BLEND_ALPHA_SUBTRACTIVE = 0xA,
    ALPHA_BLEND_ALPHA_SUBTRACTIVE_CONST = 0xB,
    ALPHA_BLEND_ALPHA_MULTIPLY = 0xC,
    ALPHA_BLEND_FILTER_PREMULT_ALPHA = 0xD,
};

/* 613 */
enum gr_clamp_mode
{
    CLAMP_MODE_WRAP = 0x0,
    CLAMP_MODE_CLAMP = 0x1,
};

/* 614 */
enum gr_zbuf_mode
{
    ZBUF_NONE = 0x0,
    ZBUF_READ = 0x1,
    ZBUF_READ_EQUAL = 0x2,
    ZBUF_WRITE = 0x3,
    ZBUF_FULL = 0x4,
};

/* 544 */
enum gr_stencil_mode
{
    STENCIL_NONE = 0x0,
    STENCIL_WRITE = 0x1,
    STENCIL_TEST = 0x2,
};

/* 546 */
enum gr_cull_mode
{
    CULL_MODE_NONE = 0x0,
    CULL_MODE_CULL = 0x1,
};

/* 547 */
enum gr_tnl_clip_mode
{
    TNL_CLIP_MODE_NONE = 0x0,
    TNL_CLIP_MODE_GUARD_BAND = 0x1,
    TNL_CLIP_MODE_CLIP = 0x2,
};

/* 548 */
enum gr_tnl_light_mode
{
    TNL_LIGHT_MODE_NONE = 0x0,
    TNL_LIGHT_MODE_LIGHT = 0x1,
};

struct gr_state
{
    unsigned int value;
    unsigned int tnl_value;
};