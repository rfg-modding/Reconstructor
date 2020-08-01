#pragma once
#include "common/Typedefs.h"
#include "keen/Threading.h"

struct index_list
{
    int head;
    int count;
};

//object_pool_server<dynamic_hash_table<unsigned int,split_page_mempool_header,&uint_hash>::hash_entry>
struct object_pool_server__dynamic_hash_table__uint_split_page_mempool_header_uint_hash__hash_entry
{
    void* base; //dynamic_hash_table<unsigned int, split_page_mempool_header, &uint_hash>::hash_entry* base;
    void* links; //index_link* links;
    char* cat;
    void* init_cb; //void(__thiscall* init_cb)(dynamic_hash_table<unsigned int, split_page_mempool_header, &uint_hash>::hash_entry* this);
    void* release_cb; //void(__thiscall* release_cb)(dynamic_hash_table<unsigned int, split_page_mempool_header, &uint_hash>::hash_entry* this);
    unsigned int max_capacity;
    index_list avail;
};

//object_pool_client<dynamic_hash_table<unsigned int,split_page_mempool_header,&uint_hash>::hash_entry,1>
struct object_pool_client__dynamic_hash_table__uint_split_page_mempool_header_uint_hash__hash_entry
{
    void* server; //object_pool_server<dynamic_hash_table<unsigned int, split_page_mempool_header, &uint_hash>::hash_entry>* server;
    index_list in_use[1];
    cs_spinlock thread_access;
    bool debug_flag;
};

struct dynamic_object_pool__dynamic_hash_table__uint_split_page_mempool_header_uint_hash__hash_entry
    : object_pool_server__dynamic_hash_table__uint_split_page_mempool_header_uint_hash__hash_entry,
      object_pool_client__dynamic_hash_table__uint_split_page_mempool_header_uint_hash__hash_entry
{

};

struct dynamic_uint_hash_table__split_page_mempool_header
{
    // : dynamic_hash_table<unsigned int,split_page_mempool_header,&uint_hash>
    //dynamic_object_pool<dynamic_hash_table<unsigned int, split_page_mempool_header, &uint_hash>::hash_entry, 1> entries;
    dynamic_object_pool__dynamic_hash_table__uint_split_page_mempool_header_uint_hash__hash_entry entries;
    void* slot; //dynamic_hash_table<unsigned int, split_page_mempool_header, &uint_hash>::hash_entry** slot;
    unsigned int size;
};

struct mempool_base;
//union $058DF3FFA7125B0957BD799298D7361D
//{
//    void(__thiscall * ~mempool_base)(mempool_base* this);
//    void* (__thiscall* __vecDelDtor)(mempool_base* this, unsigned int);
//};
//
//struct mempool_baseVtbl
//{
//    void(__thiscall* set_thread_ownership)(mempool_base* this);
//    u8 gap4[8];
//    unsigned int(__thiscall* space_free)(mempool_base* this);
//    unsigned int(__thiscall* space_used)(mempool_base* this);
//    unsigned int(__thiscall* space_max)(mempool_base* this);
//    bool(__thiscall* can_alloc)(mempool_base* this, unsigned int, unsigned int);
//    void* (__thiscall* alloc)(mempool_base* this, unsigned int, unsigned int);
//    void* (__thiscall* realloc)(mempool_base* this, void*, unsigned int);
//    bool(__thiscall* contains_address)(mempool_base* this, const void*);
//    bool(__thiscall* clear)(mempool_base* this);
//    void* (__thiscall* get_base)(mempool_base* this);
//    unsigned int(__thiscall* mark)(mempool_base* this);
//    bool(__thiscall* restore_to_mark)(mempool_base* this, unsigned int);
//    bool(__thiscall* release_bytes)(mempool_base* this, unsigned int);
//    bool(__thiscall* pad_to_page)(mempool_base* this, unsigned int);
//    $058DF3FFA7125B0957BD799298D7361D ___u14;
//};

struct split_page_mempool_header;
union $EC175F32CDCE80A72BAEB54F6A12C353
{
    split_page_mempool_header* next_page;
    unsigned int align_offset;
};

struct split_page_mempool_header
{
    unsigned int wasted_space;
    unsigned int num_pages;
    char* page_addr;
    $EC175F32CDCE80A72BAEB54F6A12C353 ___u3;
};

struct mempool_base
{
    void* vfptr;//mempool_baseVtbl* vfptr;
    volatile bool locked;
    volatile unsigned int lock_id;
    char name[32];
    char flags;
    volatile unsigned int thread_id;
    unsigned int peak_usage;
};

struct static_mempool_base : mempool_base
{
    char* last_alloc;
    char* pool;
    unsigned int max_size;
    unsigned int def_align;
    volatile unsigned int current_pos;
};

struct __declspec(align(8)) split_page_mempool : static_mempool_base
{
    unsigned int page_size;
    unsigned int max_pages;
    char* max_page_addr;
    split_page_mempool_header* first_free_page;
    unsigned int free_list_count;
    unsigned int free_list_max;
    unsigned int dbg_cur_index;
    unsigned int dbg_next_free_block;
    //Todo: Create a template for this __ is where the < goes
    dynamic_uint_hash_table__split_page_mempool_header header_hash;
    unsigned int wasted_space;
};

struct split_meminfo
{
    unsigned int size;
    unsigned int usable_size;
    unsigned int requested_size;
};

union split_memblk_unknown_union//$75DE2BBC08FE1CA7A347D0BF00F69B34
{
    const char* track;
    int in_use;
};

struct split_memblk
{
    split_meminfo info;
    split_memblk_unknown_union unknown_union;
    split_memblk* next; //16
    split_memblk* prev;
    split_memblk* top;
    split_memblk* left;
    split_memblk* right;
    char* mem_base;
    char* mem_aligned;
};

struct __declspec(align(8)) split_memmgr
{
    struct tiny_bucket
    {
        void* tiny_buf;
        unsigned int tiny_size;
        unsigned int tiny_page_size;
        unsigned int largest_alloc;
        split_page_mempool tiny_pool;
    };

    cs_wrapper cs;
    const char* name;
    void* mem_base;
    unsigned int mem_size;
    unsigned int mem_used;
    unsigned int mem_requested;
    unsigned int tiny_requested;
    split_memblk* tree_root;
    split_memblk* cached_query;
    split_memblk* last_free;
    u8 memblk_pool[64]; //dynamic_object_pool<split_memblk, 1> memblk_pool; //64
    u8 memblk_hash[72]; //dynamic_uint_hash_table<split_memblk*> memblk_hash; //72
    split_memmgr::tiny_bucket* buckets;
    int num_buckets;
    unsigned int total_tiny_space;
    split_memmgr* prev;
    split_memmgr* next;
};