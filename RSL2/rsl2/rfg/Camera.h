#pragma once
#include "Vector.h"
#include "Matrix.h"
#include "Timestamp.h"

const struct camera_view_table_entry
{
    char* name;
    int change_view_time;
    int out_time_override;
    int target_focus_speed;
    int terrain_focus_speed;
    float fov_min;
    float fov_min_sp;
    float fov_max;
    float fov_zoom_percent;
    float focal_distance_min;
    float focal_distance_max;
    float focal_range_near;
    float focal_range_far;
    float focused_distance_normal;
    float focused_distance_zoom;
    float focused_dist_target_multiplier;
    float camera_distance;
    bool use_dof;
    bool use_weapon_fov;
    bool use_far_values;
    bool use_fov_zoom;
    bool override_weapon_zoom;
    bool override_weapon_max_range;
    unsigned int effect_handle;
};

struct camera_view_data
{
    float m_camera_distance;
    float m_focused_distance;
    float m_focal_distance;
    float m_focal_range_near;
    float m_focal_range_far;
    float m_fov;
    bool m_dof_enable;
    camera_view_table_entry* m_last_view;
    camera_view_table_entry* m_current_view;
    timestamp_percent m_transition_timestamp;
    timestamp_percent m_target_zoom_timestamp;
    timestamp m_lost_target;
    unsigned int m_current_target;
    float m_temp_distance_override;
    float m_last_camera_distance;
    float m_target_camera_distance;
    float m_last_focal_distance;
    float m_target_focal_distance;
    float m_last_focused_distance;
    float m_target_focused_distance;
    float m_last_range_near;
    float m_target_range_near;
    float m_last_range_far;
    float m_target_range_far;
    float m_last_fov;
    float m_target_fov;
    float m_min_zoom;
    float m_max_zoom;
    float m_max_range;
    unsigned int m_effect_info_handle;
    unsigned int m_effect_handle;
};

enum camera_mode
{
    CAMERA_FREE_MODE = 0x0,
    CAMERA_SLEW_MODE = 0x1,
    CAMERA_CURSOR_MODE = 0x2,
    CAMERA_MINI_SCRIPT_MODE = 0x3,
    CAMERA_CUTSCENE_SCRIPT_MODE = 0x4,
    CAMERA_DUMB_MODE = 0x5,
    CAMERA_DEATH_MODE = 0x6,
    CAMERA_LOOKAROUND_MODE = 0x7,
    CAMERA_FIRST_PERSON_MODE = 0x8,
    CAMERA_TURRET_MODE = 0x9,
    CAMERA_THIRD_PERSON_MODE = 0xA,
    CAMERA_TEST_SUITE_MODE = 0xB,
    CAMERA_SATELLITE_MODE = 0xC,
    NUM_CAMERA_MODES = 0xD,
};

struct rfg_camera
{
    camera_mode mode;
    camera_mode mode_stack[8];
    int mode_stack_size;
    camera_mode level_mode;
    vector real_pos;
    vector ideal_pos;
    vector last_pos;
    matrix real_orient;
    matrix ideal_orient;
    matrix last_orient;
    float real_fov;
    float ideal_fov;
    float last_fov;
    unsigned int target_handle;
    vector last_target_pos;
    matrix last_target_orient;
    float last_aim_point_dist;
    float last_collision_fraction;
    float vertical_velocity;
    unsigned int flags;
    vector render_pos;
    matrix render_orient;

    //Note: Ignoring the rest of this struct for now so a bunch of other data we don't need yet doesn't have to be included
    //Todo: Include the rest of this struct

    //free_mode_params free_params;
    //lookaround_mode_params lookaround_params;
    //thirdperson_mode_params third_person_params;
    //mini_script_params mscript_params;
    //cutscene_mode_params cscript_params;
    //camera_view_data m_camera_view_data;
    //bool m_dof_enable;
    //float m_focus_start_A;
    //float m_focus_start_B;
    //float m_focus_end_A;
    //float m_focus_end_B;
    //float m_near_clip_dist;
    //float m_far_clip_dist;
    //float m_high_lod_far_clip_dist;
    //bool culling_frustum_valid;
    //rl_view_frustum m_culling_frustum;
};