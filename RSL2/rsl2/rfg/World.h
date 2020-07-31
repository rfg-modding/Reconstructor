#pragma once
#include "Vector.h"
#include "Matrix.h"
#include "Containers.h"
#include "Object.h"

enum world_deserialize_state
{
    DS_IDLE = 0x0,
    DS_PENDING = 0x1,
    DS_COLLECT = 0x2,
    DS_COMPRESS = 0x3,
    DS_SAVE_FINALIZE = 0x4,
};

enum world_zone_state
{
    WZS_UNLOADED = 0x0,
    WZS_STREAMING = 0x1,
    WZS_LOADED = 0x2,
};

enum world_state_mode
{
    WSM_DEFAULT = 0x0,
    WSM_CHECKPOINT = 0x1,
    NUM_WORLD_STATE_MODES = 0x2,
};

struct world_state_buf
{
    vector player_start_pos;
    matrix player_start_orient;
    vector player_start_pos_safehouse;
    matrix player_start_orient_safehouse;
    char* buf;
    int cur_size;
    int max_size;
};

struct save_load_info
{
    __int8 pending_new_game : 1;
    __int8 reset_destruction : 1;
    __int8 pending_save_game : 1;
    __int8 performing_save_game : 1;
    __int8 pending_save_state : 1;
    __int8 performing_save_state : 1;
    __int8 pending_single_zone : 1;
    __int8 save_game_warp : 1;
    __int8 game_save_checkpoint : 1;
    __int8 game_save_to_disk : 1;
    __int8 pending_load_game_from_memory : 1;
    __int8 auto_save_game : 1;
    __int8 saving_state_data : 1;
    __int8 player_start_at_safehouse : 1;
    vector player_start_pos;
    matrix player_start_orient;
};

struct __declspec(align(8)) world
{
    bool mission_object_creation_mode;
    vector level_ambient;
    vector level_back_ambient;
    char last_loaded_territory[64];
    int max_world_objects;
    base_array<Object*> all_objects;
    base_array<unsigned short> type_objects[53];
    base_array<unsigned short> subtype_objects[11];
    float tech_level;
    float tech_level_max;
    volatile unsigned int thread_id;
    char object_handle_hash[64]; //dynamic_uint_hash_table<object *> object_handle_hash; //64
    char destroyed_object_table[8198]; //hash_table<unsigned int, &hash_uint, 1024, unsigned short> destroyed_object_table; //8198
    void* save_file; //cfile* save_file; //4
    Object* flagged_objects;
    Object* current_flagged_object;
    char current_flagged_mode;
    world_deserialize_state deserialize_state;
    void* deserialize_pool; //virtual_mempool* deserialize_pool; //4
    char deserialize_list[144]; //farray<world_zone *, 32> deserialize_list; //144
    world_state_mode cur_world_state;
    world_state_buf stored_world_state[2];
    void* compress_stream; //zlib_stream* compress_stream;
    void* decompress_stream; //zlib_stream* decompress_stream;
    save_load_info sl_flags;
    void* pending_game_save_slot; //game_save_info* pending_game_save_slot;
    int dlc_bundle_id;
    char pending_filename[64];
    vector pending_game_load_warp_to_pos;
    matrix pending_game_load_warp_to_orient;
    volatile bool load_aborted;
    void* grid;//stream_grid* grid;
    vector stream_pos;
    int num_territory_zones;
    void* all_zones; //world_zone** all_zones;
    void* global_zone_grid[257]; //world_zone* global_zone_grid[257];
    bool is_territory;
    char territory_name[128];
    int num_streaming_objects;
    bool stub_serialization_in_progress;
};