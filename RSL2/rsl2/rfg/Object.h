#pragma once
#include "Vector.h"
#include "Matrix.h"

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
    char obj_type;
    char sub_type;
    vector last_known_bmin;
    vector last_known_bmax;
    unsigned int srid;
};