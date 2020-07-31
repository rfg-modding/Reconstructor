#pragma once
#include "common/Typedefs.h"
#include "Vector.h"
#include "Matrix.h"

enum object_type : u8
{
    OT_UNDEFINED = 0xFFFFFFFF,
    OT_HUMAN = 0x0,
    OT_ITEM = 0x1,
    OT_MOVER = 0x2,
    OT_VEHICLE = 0x3,
    OT_EFFECT = 0x4,
    OT_DEBRIS = 0x5,
    OT_TURRET = 0x6,
    OT_LIGHT = 0x7,
    OT_PLAYER_START = 0x8,
    OT_COVER_NODE = 0x9,
    OT_NAVPOINT = 0xA,
    OT_SQUAD = 0xB,
    OT_CONVOY = 0xC,
    OT_CONVOY_END = 0xD,
    OT_PATROL = 0xE,
    OT_GUARD_NODE = 0xF,
    OT_SKYBOX = 0x10,
    OT_LADDER = 0x11,
    OT_CONSTRAINT = 0x12,
    OT_ZONE = 0x13,
    OT_TRIGGER_REGION = 0x14,
    OT_MARAUDER_AMBUSH_REGION = 0x15,
    OT_RESTRICTED_AREA = 0x16,
    OT_SPAWN_REGION = 0x17,
    OT_AMBIENT_SPAWN_REGION = 0x18,
    OT_VEHICLE_SPAWN_NODE = 0x19,
    OT_NPC_SPAWN_NODE = 0x1A,
    OT_TURRET_SPAWN_NODE = 0x1B,
    OT_ACTION_NODE = 0x1C,
    OT_SQUAD_SPAWN_NODE = 0x1D,
    OT_ROADBLOCK_NODE = 0x1E,
    OT_SHAPE_CUTTER = 0x1F,
    OT_DISTRICT = 0x20,
    OT_MULTI_MARKER = 0x21,
    OT_PATH_ROAD = 0x22,
    OT_LIGHT_PARAMS = 0x23,
    OT_DUMMY = 0x24,
    OT_ACTIVITY_SPAWN = 0x25,
    OT_RAID_NODE = 0x26,
    OT_SUBZONE = 0x27,
    OT_HOUSE_ARREST_NODE = 0x28,
    OT_DEMOLITIONS_MASTER_NODE = 0x29,
    OT_RIDING_SHOTGUN_NODE = 0x2A,
    OT_DELIVERY_NODE = 0x2B,
    OT_BOUNDING_BOX = 0x2C,
    OT_MISSION_START_NODE = 0x2D,
    OT_COURIER = 0x2E,
    OT_COURIER_END = 0x2F,
    OT_SAFEHOUSE = 0x30,
    OT_BFTP_NODE = 0x31,
    OT_AIR_STRIKE_DEFENSE_NODE = 0x32,
    OT_UPGRADE_NODE = 0x33,
    OT_AREA_DEFENSE_NODE = 0x34,
    NUM_OBJECT_TYPES = 0x35,
};

enum object_sub_type : u8
{
    OT_SUB_UNDEFINED = 0xFFFFFFFF,
    OT_SUB_MOVER_GENERAL = 0x0,
    OT_SUB_MOVER_RFG = 0x1,
    OT_SUB_HUMAN_NPC = 0x2,
    OT_SUB_HUMAN_PLAYER = 0x3,
    OT_SUB_VEHICLE_AUTO = 0x4,
    OT_SUB_VEHICLE_FLYER = 0x5,
    OT_SUB_VEHICLE_WALKER = 0x6,
    OT_SUB_ITEM_WEAPON = 0x7,
    OT_SUB_ITEM_PROJECTILE = 0x8,
    OT_SUB_ITEM_MULTI_FLAG = 0x9,
    OT_SUB_ITEM_MULTI_BACKPACK = 0xA,
    NUM_OBJECT_SUB_TYPES = 0xB,
};

struct ContactNode //16
{
    unsigned int m_contacted_object; //4
    unsigned __int16 m_num_contacts; //4
    ContactNode* prev; //4
    ContactNode* next; //4
};

struct object_contact_info //4
{
    ContactNode* m_contact_list; //4
};

struct remote_object_flags //5
{
	__int8 remote_object : 1;
	__int8 client_sync : 1;
	__int8 client_destroy_permission : 1;
	__int8 no_destroy_packet : 1;
	__int8 common_create : 1;
};

struct object_flags //23 * 1 = 23 Bytes
{
	__int8 flagged_list_state : 3; //1 - offset = 84
	__int8 lighting_set_once : 1; //1 - offset = 85
	__int8 destroyed : 1; // - offset = 86
	__int8 no_save : 1;
	__int8 force_full_save : 1;
	__int8 destroy_on_stream : 1;
	__int8 created_by_mission_or_activity : 1;
	__int8 dont_transform : 1;
	__int8 world_free : 1;
	__int8 streaming : 1;
	__int8 streamed : 1;
	__int8 persistent : 1;
	__int8 original : 1;
	__int8 stub : 1;
	__int8 preserve_handle : 1;
	__int8 bpo_index : 2;
	__int8 is_dependent : 1;
	__int8 visited : 1;
	__int8 special_lifetime : 1;
	__int8 serialize_protected : 1;
	__int8 _dont_use_me : 1;
	__int8 streaming_fixed : 1;
	__int8 render_flags : 4;
};

struct Object
{
    //objectVtbl* vfptr;
    void* vfptr;
    vector pos;
    matrix orient;
    Object* child_ptr;
    Object* child_next;
    Object* child_prev;

    //constraint* child_constraint_ptr;
    //constraint* host_constraint_ptr;
    //attach_info_data* attach_info;
    void* child_constraint_ptr;
    void* host_constraint_ptr;
    void* attach_info;

    unsigned int havok_handle;
    object_contact_info contact_info;
    object_flags obj_flags;
    remote_object_flags remote_obj_flags;
    char mp_dcmo_index;
    int checking_reset;
    unsigned __int16 name_index;
    Object* flagged_next;
    Object* flagged_prev;
    unsigned int handle;
    unsigned int parent;
    //havok_bpo* bp_handle;
    void* bp_handle;
    unsigned __int16 all_index;
    unsigned __int16 type_index;
    unsigned __int16 subtype_index;
    object_type obj_type;
    object_sub_type sub_type;
    vector last_known_bmin;
    vector last_known_bmax;
    unsigned int srid;
};

static_assert(sizeof(Object) == 156, "Object failed the static size check!");