#pragma once
#include "common/patching/FunHook.h"
#include "common/Typedefs.h"
#include "rfg/Player.h"
#include "rsl2/misc/GlobalState.h"
#include "rsl2/util/Util.h"
#include "rsl2/functions/Functions.h"
#include "RFGR_Types/rfg/stream2/stream2.h"
#include "RFGR_Types/rfg/keen/Threading.h"
#include "common/BuildConfig.h"
#ifdef COMPILE_IN_PROFILER
#include "tracy/Tracy.hpp"
#endif

//bool __cdecl assasm_start(const char *system_name, mempool_base *tmp_pool, mempool_base *container_pool,
//                          bool load_from_preload, mempool_base *peg_pool, bool build_peg, bool is_temp, bool validate,
//                          bool patch_data //0x003B9A60
FunHook<bool(const char* system_name, mempool_base* tmp_pool, mempool_base* container_pool, bool load_from_preload, mempool_base* peg_pool, bool build_peg, bool is_temp, bool validate, bool patch_data)> assasm_start_hook
{
    0x003B9A60,
    [](const char* system_name, mempool_base* tmp_pool, mempool_base* container_pool, bool load_from_preload, mempool_base* peg_pool, bool build_peg, bool is_temp, bool validate, bool patch_data) -> bool
    {
        bool result = assasm_start_hook.CallTarget(system_name, tmp_pool, container_pool, load_from_preload, peg_pool, build_peg, is_temp, validate, patch_data);
        printf("\nassasm_start_hook::\n");
        printf("\tsystem_name: %s\n", system_name);
        printf("\ttmp_pool.name: %s\n", tmp_pool->name);
        printf("\tcontainer_pool.name: %s\n", container_pool->name);
        printf("\tload_from_preload: %s\n", load_from_preload ? "true" : "false");
        printf("\tpeg_pool.name: %s\n", peg_pool->name);
        printf("\tbuild_peg: %s\n", build_peg ? "true" : "false");
        printf("\tis_temp: %s\n", is_temp ? "true" : "false");
        printf("\tvalidate: %s\n", validate ? "true" : "false");
        printf("\tpatch_data: %s\n", patch_data ? "true" : "false");
        return result;
    }
};

//bool __cdecl assasm_finish(pasm_data *peg_data, bool verify_only, bool pack_pointers, bool force_no_data_generate) //0x003B9C30
FunHook<bool(pasm_data* peg_data, bool verify_only, bool pack_pointers, bool force_no_data_generate)> assasm_finish_hook
{
    0x003B9C30,
    [](pasm_data* peg_data, bool verify_only, bool pack_pointers, bool force_no_data_generate) -> bool
    {
        bool result = assasm_finish_hook.CallTarget(peg_data, verify_only, pack_pointers, force_no_data_generate);
        printf("\nstream2_finish::\n");
        printf("\tverify_only: %s\n", verify_only ? "true" : "false");
        printf("\tpack_pointers: %s\n", pack_pointers ? "true" : "false");
        printf("\tforce_no_data_generate: %s\n", force_no_data_generate ? "true" : "false");
        return result;
    }
};

//bool __cdecl assasm_container_start(const char *widget_name, char container_type, bool preload, bool passive) //0x003B9BA0
FunHook<bool(const char* widget_name, char container_type, bool preload, bool passive)> assasm_container_start
{
    0x003B9BA0,
    [](const char* widget_name, char container_type, bool preload, bool passive) -> bool
    {
        bool result = assasm_container_start.CallTarget(widget_name, container_type, preload, passive);
        printf("\nassasm_container_start::\n");
        printf("\twidget_name: %s\n", widget_name);
        printf("\tcontainer_type: %d\n", container_type);
        printf("\tpreload: %s\n", preload ? "true" : "false");
        printf("\tpassive: %s\n", passive ? "true" : "false");
        return result;
    }
};

//unsigned int __cdecl assasm_container_finish(bool no_preload, bool is_temp, bool patch) //0x003CB040
FunHook<u32(bool no_preload, bool is_temp, bool patch)> assasm_container_finish
{
    0x003CB040,
    [](bool no_preload, bool is_temp, bool patch) -> u32
    {
        u32 result = assasm_container_finish.CallTarget(no_preload, is_temp, patch);
        printf("\nassasm_container_finish::\n");
        printf("\tno_preload: %s\n", no_preload ? "true" : "false");
        printf("\tis_temp: %s\n", is_temp ? "true" : "false");
        printf("\tpatch: %s\n", patch ? "true" : "false");
        return result;
    }
};

//bool __cdecl stream2_container_start(const char *name, mempool_base *pool, char tpe, bool passive, bool force_no_stream_generation) //0x000AC3F0
FunHook<bool(const char* name, mempool_base* pool, char type, bool passive, bool force_no_stream_generation)> stream2_container_start_hook
{
    0x000AC3F0,
    [](const char* name, mempool_base* pool, char type, bool passive, bool force_no_stream_generation) -> bool
    {
        bool result = stream2_container_start_hook.CallTarget(name, pool, type, passive, force_no_stream_generation);
        printf("\nstream2_container_start_hook::\n");
        printf("\tname: %s\n", name);
        printf("\tpool.name: %s\n", pool->name);
        printf("\ttype: %d\n", type);
        printf("\tpassive: %s\n", passive ? "true" : "false");
        printf("\tforce_no_stream_generation: %s\n", force_no_stream_generation ? "true" : "false");
        return result;
    }
};

//stream2_prim *__cdecl stream2_container_add_prim(const char *filename, const char *category, char tpe, char allocator,
//                                                 bool report_errors_immediately, char add_flags, stream2_prim_record *precalc) //0x000B02C0
FunHook<stream2_prim*(const char* filename, const char* category, char type, char allocator, bool report_errors_immediately, char add_flags, stream2_prim_record* precalc)> stream2_container_add_prim_hook
{
    0x000B02C0,
    [](const char* filename, const char* category, char type, char allocator, bool report_errors_immediately, char add_flags, stream2_prim_record* precalc) -> stream2_prim*
    {
        stream2_prim* result = stream2_container_add_prim_hook.CallTarget(filename, category, type, allocator, report_errors_immediately, add_flags, precalc);
        printf("\nstream2_container_add_prim_hook2::\n");
        printf("\tfilename: %s\n", filename);
        printf("\tcategory: %s\n", category);
        printf("\ttype: %d\n", type);
        printf("\tallocator: %d\n", allocator);
        printf("\treport_errors_immediately: %s\n", report_errors_immediately ? "true" : "false");
        printf("\tadd_flags: %d\n", add_flags);
        if (precalc)
            printf("\tprim record name: %s\n", precalc->name);

        return result;
    }
};

//unsigned int __cdecl stream2_container_finish(bool skip_allocator_size_verification, bool patch) //0x000B0EC0
FunHook<u32(bool skip_allocator_size_verification, bool patch)> stream2_container_finish_hook
{
    0x000B0EC0,
    [](bool skip_allocator_size_verification, bool patch) -> u32
    {
        u32 result = stream2_container_finish_hook.CallTarget(skip_allocator_size_verification, patch);
        printf("\nstream2_container_finish_hook::\n");
        printf("\tskip_allocator_size_verification: %s\n", skip_allocator_size_verification ? "true" : "false");
        printf("\tpatch: %s\n", patch ? "true" : "false");
        return result;
    }
};

//Unload containers queued for unload
void queue_process_unload();
void dequeue(stream2_container* container);
void queue_process();
void queue_process_passive();
//Rewrite of stream2_container::stream_wait(). Not doing this in a hook since only queue_process() calls it, which was also rewritten.
bool stream2_container_stream_wait(stream2_container* container);

//bool __cdecl stream2_do_frame(bool do_passive) //0x000B0DB0
FunHook<bool(bool do_passive)> stream2_do_frame_hook
{
    0x000B0DB0,
    [](bool do_passive) -> bool
    {
        static RSL2_GlobalState* globalState = GetGlobalState();
        static rfg::RfgFunctions* functions = GetRfgFunctions();

        //Only process streaming on the stream thread
        if (functions->get_current_thread_id() != *globalState->Stream_thread_id)
            return true;

        //Get top of each stream queue?
        stream2_container** stream_q = globalState->Stream_q;
        stream2_container** stream_q_passive = globalState->Stream_q_passive;

        //Process queue
        queue_process_unload(); //functions->queue_process_unload();
        queue_process(); //functions->queue_process();

        //Process passive queue? Maybe this is for low priority streaming?
        if (do_passive)
            queue_process_passive(); //functions->queue_process_passive();

        //Return false if both queues are empty?
        if (!(*stream_q) && (!do_passive || !(*stream_q_passive)))
            return false;

        //Success
        return true;


        //Empty hook
        //bool result = stream2_do_frame_hook.CallTarget(do_passive);
        //return result;
    }
};

//Todo: Must hook these to fully replace usage of queue_process_unload(), queue_process(), and queue_process_passive()::
//    - stream2_preload()
//    - stream2_container_delete()
//    - stream2_system_shutdown()

//Reimplementation of queue_process_unload() so we can replace the streaming system. All functions that call this are hooked and changed to use this variant.
void queue_process_unload()
{
    static RSL2_GlobalState* globalState = GetGlobalState();
    static rfg::RfgFunctions* functions = GetRfgFunctions();

    //Unload list root node
    stream2_container** stream_q_unload = globalState->Stream_q_unload;

    //Iterate list of containers and unload each
    stream2_container* current = *stream_q_unload;
    while (current)
    {
        //Get next node
        stream2_container* next = nullptr;
        if (current->q_next == *stream_q_unload)
            next = nullptr; //Reached end of linked list (loops around to the first element)
        else
            next = current->q_next;

        //Unload container
        functions->stream2_container_unload(current, nullptr); //stream2_container::unload(current);
        current->flags &= 65531; //TODO: Zero out Flag1

        //Clear current node and update next/prev pointers
        if (current == *stream_q_unload)
        {
            //If current node is the root also clear that
            *stream_q_unload = current->q_next;
            if (current == *stream_q_unload)
            {
                *stream_q_unload = nullptr;
                current->q_prev = nullptr;
                current->q_next = nullptr;
                current = next;
            }
        }
        else
        {
            current->q_prev->q_next = current->q_next;
            current->q_next->q_prev = current->q_prev;
            current->q_prev = nullptr;
            current->q_next = nullptr;
            current = next;
        }
    }
}

void dequeue(stream2_container* container)
{
    static RSL2_GlobalState* globalState = GetGlobalState();
    static rfg::RfgFunctions* functions = GetRfgFunctions();
    stream2_container** stream_q = globalState->Stream_q;
    stream2_container** stream_q_passive = globalState->Stream_q_passive;

    const bool passive = (container->flags & stream2_container::Flags::Passive) != 0; //Likely signals that the container is passive if true, whatever that means.
    if (!passive)
    {
        if (*stream_q == container)
        {
            *stream_q = container->q_next;
            if (*stream_q == container)
            {
                //Only 1 element left in list
                *stream_q = nullptr;
                container->flags &= 65533; //TODO: Zero out Flag1
                container->q_next = nullptr;
                container->q_prev = nullptr;
                return;
            }
        }
    }
    else if (*stream_q_passive == container)
    {
        *stream_q_passive = container->q_next;
        if (*stream_q_passive == container)
        {
            //Only 1 element left in list
            *stream_q_passive = nullptr;
            container->flags &= 65533; //TODO: Zero out Flag1
            container->q_next = nullptr;
            container->q_prev = nullptr;
            return;
        }
    }

    //Remove container from linked list and connect prev/next nodes
    container->q_prev->q_next = container->q_next;
    container->q_next->q_prev = container->q_prev;
    container->flags &= 65533; //TODO: Zero out Flag1
    container->q_next = nullptr;
    container->q_prev = nullptr;
}

void queue_process()
{
    static RSL2_GlobalState* globalState = GetGlobalState();
    static rfg::RfgFunctions* functions = GetRfgFunctions();

    //Top of container stream queue?
    stream2_container** stream_q = globalState->Stream_q;
    bool* stream_q_paused = globalState->Stream_q_paused;
    bool* re_entrant = globalState->Re_entrant;

    //No idea what this flag does. This sort of thing usually happens when a function is recursive but this isn't. It's also only used in this function.
    //Possibly prevents the function from doing anything again if an error occurs (e.g. container allocation fails)
    if (*re_entrant == true)
        return;

    *re_entrant = true;

    if (*stream_q)
    {
        bool flag3 = ((*stream_q)->flags & 8) != 0;
        bool flag4 = ((*stream_q)->flags & 16) != 0;
        if (!flag4)
        {
            if (flag3)
            {
                bool result = stream2_container_stream_wait(*stream_q);//functions->stream2_container_stream_wait(*stream_q, nullptr);
                if (result)
                {
                    *re_entrant = false;
                    return;
                }
                dequeue(*stream_q);
            }
        }
        else
        {
            bool result = functions->stream2_container_abort_wait(*stream_q, nullptr);
            if (result)
            {
                *re_entrant = false;
                return;
            }
            dequeue(*stream_q);
        }

        while (*stream_q && !(*stream_q_paused))
        {
            //Allocate space for container data
            stream2_container* current = *stream_q;
            bool container_alloc_result = functions->stream2_container_alloc(*stream_q, nullptr);
            if (!container_alloc_result)
            {
                //Allocation failure
                current->flags = (stream2_container::Flags)(current->flags & 65527U | 64); //Todo: Simplify this
                dequeue(*stream_q);
                if (stream_q)
                {
                    continue;
                }
                else
                {
                    *re_entrant = false;
                    return;
                }
            }

            //Load container data
            bool flag7 = ((*stream_q)->flags & 128) != 0;
            if (!flag7)
            {
                current->pf_chain_size = 0;
                for (i32 i = 0; i < current->num_entries; i++)
                {
                    stream2_container::entry* entry = &current->entries[i];
                    stream2_prim* primitive = entry->prim;
                    bool entryEnabled = (entry->enable & 1) != 0;
                    bool primFlag0 = (primitive->flags & 1) != 0;
                    if (entryEnabled && !primFlag0)
                    {
                        stream2_os_file cpuFile;
                        cpuFile.chain_read = 0;
                        cpuFile.encrypted = 0;

                        //Set os file defaults
                        cpuFile.filename[0] = '\0';
                        //cpuFile.size = 0;
                        //cpuFile.dest = nullptr;
                        cpuFile.chain_num_reads = 0;
                        cpuFile.chain_size = nullptr;
                        cpuFile.chain_dest = nullptr;
                        //cpuFile.priority = 0;
                        cpuFile.preopened = nullptr;
                        cpuFile.offset = 0;
                        cpuFile.size = primitive->cpu_size;
                        cpuFile.dest = primitive->cpu_data;
                        cpuFile.priority = current->cur_priority;

                        bool primFlag2 = (primitive->flags & 4) != 0;
                        if (!primFlag2)
                        {
                            //Copy primitive->name to cpuFile.filename
                            char* src = primitive->name;
                            char* dst = cpuFile.filename;
                            char c = *src;
                            while (c != '\0')
                            {
                                c = *src;
                                *dst = c;
                                src++;
                                dst++;
                            }
                        }
                        else
                        {
                            //Add extensions based on file type (cpu/gpu file)
                            functions->cf_add_ext(cpuFile.filename, 64, primitive->name, primitive->pdef->cpu_ext[primitive->split_ext_index], false);
                        }

                        //Read primitive data?
                        void* stream2_os_file_read_result = (void*)functions->stream2_os_read(&cpuFile);
                        current->pf_chain_read_dest[current->pf_chain_size] = stream2_os_file_read_result;
                        current->pf_chain_size++;

                        //Load gpu file if primitive is split
                        primFlag2 = (primitive->flags & 4) != 0; //Update again just in case
                        if (primFlag2 && primitive->gpu_size > 0)
                        {
                            //TODO: Do all this init in the stream2_os_file constructor, only set values that are different from default here.
                            stream2_os_file gpuFile;
                            //gpuFile.priority = 0;
                            gpuFile.filename[0] = '\0';
                            //gpuFile.size = 0;
                            //gpuFile.dest = nullptr;
                            gpuFile.chain_num_reads = 0;
                            gpuFile.chain_size = nullptr;
                            gpuFile.chain_dest = nullptr;
                            gpuFile.preopened = nullptr;
                            gpuFile.size = primitive->gpu_size;
                            gpuFile.offset = 0;
                            gpuFile.priority = current->cur_priority;
                            functions->cf_add_ext(gpuFile.filename, 64, primitive->name, primitive->pdef->gpu_ext[primitive->split_ext_index], false);

                            stream2_prim_def* primDef = primitive->pdef;
                            if (primDef->flags.alloc_split == 0) //Cpu file and gpu file data stored in the same buffer
                            {
                                //Calculate alignment padding between cpu and gpu data
                                const i32 remainder = primitive->cpu_size % primDef->gpu_align;

                                //Set gpu file data ptr
                                u32 alignPad = (remainder == 0) ? 0 : primDef->gpu_align - remainder;
                                gpuFile.dest = primitive->cpu_data + primitive->cpu_size + alignPad;
                            }
                            else //Separate buffers for cpu file and gpu file data
                            {
                                gpuFile.dest = primitive->gpu_data;
                            }

                            void* gpuFileReadResult = (void*)functions->stream2_os_read(&gpuFile);
                            current->pf_chain_read_dest[current->pf_chain_size] = gpuFileReadResult;
                            current->pf_chain_size = current->pf_chain_size + 1;
                        }

                    }
                }
            }
            else
            {
                stream2_os_file cpuFile;
                cpuFile.chain_read = 1;
                cpuFile.encrypted = 0;

                //cpuFile.priority = 0;
                cpuFile.filename[0] = '\0';
                cpuFile.size = 0;
                cpuFile.dest = nullptr;
                //cpuFile.chain_num_reads = 0;
                cpuFile.chain_size = nullptr;
                cpuFile.chain_dest = nullptr;
                cpuFile.preopened = nullptr;
                cpuFile.offset = current->pf_base_offset;
                cpuFile.priority = current->cur_priority;
                cpuFile.size = current->pf_total_compressed_read_size;
                cpuFile.chain_size = current->pf_chain_read_size;
                cpuFile.chain_dest = (char**)(current->pf_chain_read_dest + 1);
                cpuFile.chain_num_reads = 0;
                sprintf(cpuFile.filename, "%s%s", current->name, ".str2_pc");

                //Add primitives to read chain?
                for (i32 i = 0; i < current->num_entries; i++)
                {
                    i32 chain_num_reads = cpuFile.chain_num_reads;
                    stream2_container::entry* entry = &current->entries[i];
                    bool entryEnabled = (entry->enable & 1) != 0;
                    if (entryEnabled)
                    {
                        stream2_prim* primitive = entry->prim;
                        bool primFlag0 = (primitive->flags & 1) != 0;
                        char* data = primFlag0 ? nullptr : primitive->cpu_data;

                        cpuFile.chain_dest[cpuFile.chain_num_reads] = data;
                        chain_num_reads = cpuFile.chain_num_reads + 1;

                        //Flag seems to signal if the primitive is split into two files (cpu/gpu files)
                        bool primFlag2 = (primitive->flags & 4) != 0;
                        if (primFlag2 && primitive->gpu_size > 0)
                        {
                            data = primFlag0 ? nullptr : primitive->gpu_data;
                            cpuFile.chain_dest[chain_num_reads] = data;
                            chain_num_reads = cpuFile.chain_num_reads + 2;
                        }
                    }

                    cpuFile.chain_num_reads = chain_num_reads;
                }

                //Read file data
                void* out = (void*)functions->stream2_os_read(&cpuFile);
                *(current->pf_chain_read_dest) = out;
            }

            current->flags |= 8; //Likely is true when the container has been read or is currently being read
            break;
        }
    }

    *re_entrant = false;
}

//TODO: NEXT::
//    - Set names for stream2 container flags and use them instead of numbers when possible
//    - Check out the container load callbacks in game_stream2_register_types. I thought there were only primitive load callbacks
//    - Update comments to remove ?s and other non concrete / out of date info
//    - Remove extraneous stream2 container flag operators below stream2_contaiener def
//    - Figure out what it means for a container to be passive. Name doesn't make much sense so far.
//    - Implement this function and test it
//    - Locate where the file load functions are called and see there's some way to intercept them and use loose files as replacements. Maybe need to patch packfile/container/asm/prim defs/lists
//    - Figure out if there's some way to add custom vpp_pcs generated by mods for file overrides so they can use the speed advantages for release mode mods
//    - Figure out if there's some way to do runtime selective xtbl replacements so mods could just put their entry mods for different xtbls in their own folders
//          - See if this has any limits over the pre generated edits approach used by the MM
//          - See how this would integrate with Nanoforge
//    - Rewrite world::set_pending_territory_load, world::init_territory and other streaming functions



//TODO: **FIRST** Seems the stream2_container_load() handles calling primitive load callbacks
//    - See feasibility of using just this to implement global loose file swapping. Possibly would need to update container size data and patch it if the default was used later
//    - Consider what would be required to support hot reload on all files. I'm guessing it'd need to be implemented for every format and separately for each xtbl
//TODO: Determine if this function does anything. May be able to call the container load_cb in some cases
//TODO: Maybe remove code that's impossible to be called
void queue_process_passive()
{
    static RSL2_GlobalState* globalState = GetGlobalState();
    static rfg::RfgFunctions* functions = GetRfgFunctions();

    //Top of passive container queue
    stream2_container** stream_q_passive = globalState->Stream_q_passive;

    //Todo: Try to make this a for loop
    stream2_container* next = *stream_q_passive;
    while(true)
    {
    root_loop:
        //Ensure there's a valid container
        stream2_container* current = next; //Todo: Should I just use stream_q_passive directly?
        if (!current)
            return;

        //Set container for next iteration
        if (current->q_next == *stream_q_passive)
            next = nullptr; //Reached end of linked list (loops around to the first element)
        else
            next = current->q_next;

        //TODO: How the fuck can the two loops after this one ever work? It skips a container if (enabled &&!primFlag0), those loops require that to process containers
        //Skip container if entries don't have valid flags for loading
        for (u32 i = 0; i < current->num_entries; i++)
        {
            stream2_container::entry* entry = &current->entries[i];
            bool enabled = (entry->enable & 1) != 0;
            bool primFlag0 = (entry->prim->flags & 1) != 0; //Todo: Determine what this flag signifies
            if (enabled && !primFlag0)
                goto root_loop;
        }

        //Load entries??
        for (u32 i = 0; i < current->num_entries; i++)
        {
            stream2_container::entry* entry = &current->entries[i];
            bool enabled = (entry->enable & 1) != 0;
            if (enabled)
            {
                stream2_prim* primitive = entry->prim;
                bool primFlag0 = (primitive->flags & 1) != 0;
                bool primFlag2 = (primitive->flags & 4) != 0; //Todo: Determine what this signifies
                bool loadThreaded = primitive->pdef->flags.load_threaded != 0; //Todo: Determine what this signifies
                if (!primFlag0 && loadThreaded) //Todo: How can this ever be reached? The previous loop skips the container if (enabled && !primFlag0)
                {
                    //Get gpu data and primitive ID
                    u32 gpuDataSize = primFlag2 ? primitive->gpu_size : 0;
                    char* gpuData = primFlag2 ? primitive->gpu_data : nullptr;
                    u32 srid = (u32)current; //SRID is just the address of the stream2_container that holds the primitive

                    //Call the primitive load callback. Data is already loaded into memory at this point.
                    bool loadResult = primitive->pdef->load_cb(primitive->name, srid, primitive->cpu_data, primitive->cpu_size, gpuData, gpuDataSize);
                    if (!loadResult)
                        break;
                    else
                        primitive->flags |= 1; //Set primFlag0 to true
                }
            }
        }

        for (u32 i = 0; i < current->num_entries; i++)
        {
            //Todo: See if this or the previous loop can ever call a load callback given that the flags would cause the first loop to skip the container
            stream2_container::entry* entry = &current->entries[i];
            bool enabled = (entry->enable & 1) != 0;
            if (enabled)
            {
                stream2_prim* primitive = entry->prim;
                bool primFlag0 = (primitive->flags & 1) != 0;
                bool primFlag2 = (primitive->flags & 4) != 0;
                bool loadThreaded = primitive->pdef->flags.load_threaded != 0;
                if (!primFlag0 && !loadThreaded) //TODO: [CRITICAL] Ensure loadThreaded is being used here. The game does some bitfuckery that should be equivalent to (pdef->flags & 4) != 0
                {
                    //Get gpu data and stream resource id
                    u32 gpuDataSize = primFlag2 ? primitive->gpu_size : 0;
                    char* gpuData = primFlag2 ? primitive->gpu_data : nullptr;
                    u32 srid = (u32)current; //SRID is just the address of the stream2_container that holds the primitive

                    //Call the primitive load callback. Data is already loaded into memory at this point.
                    bool loadResult = primitive->pdef->load_cb(primitive->name, srid, primitive->cpu_data, primitive->cpu_size, gpuData, gpuDataSize);
                    if (!loadResult)
                    {
                        current->flags |= stream2_container::Flags::Flag0; //current->flags |= 1;
                        dequeue(current); //Todo: Ensure the correct thing is being passed. Might need to pass *Stream_q_passive instead
                        goto root_loop;
                    }
                    else
                    {
                        primitive->flags |= 1;
                    }
                }
            }
        }

        //Call container load callback
        if (current->cdef->load_cb)
            current->cdef->load_cb((u32)current);

        current->flags |= stream2_container::Flags::Flag0; //current->flags |= 1;
        dequeue(current); //Todo: Ensure the correct thing is being passed. Might need to pass *Stream_q_passive instead
    }
}


//bool __thiscall world::set_pending_territory_load(world *this, const char *filename, bool new_game, bool single_zone,
//                                                  bool player_start_at_safehouse, int dlc_id, bool reset_destruction) //0x0051C320
FunHook<bool __fastcall(world* thisPtr, void* edx, const char* filename, bool new_game, bool single_zone,
                        bool player_start_at_safehouse, int dlc_id, bool reset_destruction)> world_set_pending_territory_load_hook
{
    0x0051C320,
    [](world* thisPtr, void* edx, const char* filename, bool new_game, bool single_zone, bool player_start_at_safehouse, int dlc_id, bool reset_destruction) -> bool
    {
        //static RSL2_GlobalState* globalState = GetGlobalState();
        //static rfg::RfgFunctions* functions = GetRfgFunctions();
        //*globalState->Multiplayer = false;

        ////filename = "dlc01";
        ////single_zone = false;
        ////new_game = true;
        ////thisPtr->is_territory = true;
        //bool result = world_set_pending_territory_load_hook.CallTarget(thisPtr, edx, filename, new_game, single_zone, player_start_at_safehouse, dlc_id, reset_destruction);
        //printf("\nworld::set_pending_territory_load::\n");
        //printf("\tfilename: %s\n", filename);
        //printf("\tnew_game: %s\n", new_game ? "true" : "false");
        //printf("\tsingle_zone: %s\n", single_zone ? "true" : "false");
        //printf("\tplayer_start_at_safehouse: %s\n", player_start_at_safehouse ? "true" : "false");
        //printf("\tdlc_id: %d\n", dlc_id);
        //printf("\treset_destruction: %s\n", reset_destruction ? "true" : "false");
        //printf("\tresult: %s\n", result ? "true" : "false");
        //return result;
        return world_set_pending_territory_load_hook.CallTarget(thisPtr, edx, filename, new_game, single_zone, player_start_at_safehouse, dlc_id, reset_destruction);
    }
};

//world_zone *__thiscall world::init_territory(world *this, const char *filename, bool new_game, bool single_zone) //0x0053FCE0
FunHook<world_zone* __fastcall(world* thisPtr, void* edx, const char* filename, bool new_game, bool single_zone)> world_init_territory_hook
{
    0x0053FCE0,
    [](world* thisPtr, void* edx, const char* filename, bool new_game, bool single_zone) -> world_zone*
    {
        //thisPtr->is_territory = true;
        //filename = "terr01";
        //world_zone* result = world_init_territory_hook.CallTarget(thisPtr, edx, filename, new_game, single_zone);
        //printf("\nworld::init_territory::\n");
        //printf("\tfilename: %s\n", filename);
        //printf("\tnew_game: %s\n", new_game ? "true" : "false");
        //printf("\tsingle_zone: %s\n", single_zone ? "true" : "false");
        //return result;
        return world_init_territory_hook.CallTarget(thisPtr, edx, filename, new_game, single_zone);
    }
};

//int __cdecl stream2_container_load(keen::Thread *thread) //0x000AF630
FunHook<int __cdecl(keen::Thread* thread)> stream2_container_load_hook
{
    0x000AF630,
    [](keen::Thread* thread) -> int
    {
        static RSL2_GlobalState* globalState = GetGlobalState();
        static rfg::RfgFunctions* functions = GetRfgFunctions();

        auto* argumentFunc = (void(*)())thread->m_pArgument;
        argumentFunc(); //Todo: Determine what this even does
        if (thread->m_quitRequested)
            return 0;

        while (true)
        {
            functions->keen_Event_sleepUntilSignaled(globalState->g_stream2ContainerLoadEvent, nullptr);
            stream2_container** container = globalState->g_pStream2ContainerToLoad;
            if (*container)
            {
                for (u32 i = 0; i < (*container)->num_entries; i++)
                {
                    stream2_container::entry* entry = &(*container)->entries[i];
                    bool enabled = (entry->enable & 1) != 0;
                    if (enabled)
                    {
                        stream2_prim* primitive = entry->prim;
                        bool primFlag0 = (primitive->flags & 1) != 0;
                        bool primFlag2 = (primitive->flags & 4) != 0;
                        bool loadThreaded = primitive->pdef->flags.load_threaded != 0;
                        if (!primFlag0 && loadThreaded)
                        {
                            //Get gpu data and primitive ID
                            u32 gpuDataSize = primFlag2 ? primitive->gpu_size : 0;
                            char* gpuData = primFlag2 ? primitive->gpu_data : nullptr;
                            u32 srid = (u32)(*container); //SRID is just the address of the stream2_container that holds the primitive

                            bool loadResult = primitive->pdef->load_cb(primitive->name, srid, primitive->cpu_data, primitive->cpu_size, gpuData, gpuDataSize);
                            if (!loadResult)
                            {
                                (*container)->flags |= (stream2_container::Flags)64;
                                break;
                            }
                            primitive->flags |= 1;
                        }
                    }
                }

                *globalState->g_stream2ContainerLoaded = true;
            }
            if (thread->m_quitRequested)
                return 0;
        }
    }
};

//TODO: Next: Log to a file every time a primitive/container load cb is called and where it was called from. Try to simplify code and remove useless stuff. Test SP, DLC, MP, and WC
//TODO: Document flags and their behavior
//TODO: Try to figure out how packfiles fit into this. Maybe look for the code that actually reads the data into the cpu/gpu file buffers. Likely related to stream2_os_request && stream2_os_device
//TODO: Try to start documenting how the streaming system works
//TODO: Do the other todos further up in this file
bool stream2_container_stream_wait(stream2_container* container)
{
    static RSL2_GlobalState* globalState = GetGlobalState();
    static rfg::RfgFunctions* functions = GetRfgFunctions();

    //Check streaming status
    stream2_os_req_status status = functions->stream2_container_req_poll(container, nullptr); //TODO: IMPLEMENT
    if (status == SOS_BUSY)
        return true;
    if (status == SOS_DONE_ERROR)
        functions->stream2_container_abort_wait(container, nullptr);

    //Done streaming the container. Signal loading thread to load the containers primitives.
    if (*globalState->g_pStream2ContainerToLoad == nullptr)
    {
        *globalState->g_stream2ContainerLoaded = false;
        *globalState->g_pStream2ContainerToLoad = container;
        functions->keen_Event_signal(globalState->g_stream2ContainerLoadEvent, nullptr); //TODO: IMPLEMENT
        return true;
    }

    //Load primitives //TODO: [CRITICAL] Figure out why this and the load thread call the load CBs. Seems like it might depend on the load_threaded flag, despite streaming being single threaded.
    if (*globalState->g_pStream2ContainerToLoad == container && *globalState->g_stream2ContainerLoaded == true)
    {
        for (u32 i = 0; i < container->num_entries; i++)
        {
            stream2_container::entry* entry = &container->entries[i];
            bool enabled = (entry->enable & 1) != 0;
            if (enabled)
            {
                stream2_prim* primitive = entry->prim;
                bool primFlag0 = (primitive->flags & 1) != 0;
                bool primFlag2 = (primitive->flags & 4) != 0;
                bool loadThreaded = primitive->pdef->flags.load_threaded != 0;
                if (!primFlag0 && !loadThreaded) //TODO: [CRITICAL] Ensure loadThreaded is being used here. The game does some bitfuckery that should be equivalent to (pdef->flags & 4) != 0
                {
                    //Get gpu data and primitive ID
                    u32 gpuDataSize = primFlag2 ? primitive->gpu_size : 0;
                    char* gpuData = primFlag2 ? primitive->gpu_data : nullptr;
                    u32 srid = (u32)(container); //SRID is just the address of the stream2_container that holds the primitive

                    bool loadResult = primitive->pdef->load_cb(primitive->name, srid, primitive->cpu_data, primitive->cpu_size, gpuData, gpuDataSize);
                    if (!loadResult)
                    {
                        container->flags |= stream2_container::Flags::Flag0;
                        functions->stream2_container_req_release(container, nullptr);
                        if (status == SOS_DONE_ERROR)
                            container->flags |= stream2_container::Flags::Flag6;

                        container->flags &= (stream2_container::Flags)65527; //TODO: Simplify this. Likely zero's some lower bits
                        *globalState->g_pStream2ContainerToLoad = nullptr;
                        return false;
                    }
                    primitive->flags |= 1;
                }
            }
        }

        if (container->cdef->load_cb)
            container->cdef->load_cb((u32)container);

        container->flags |= stream2_container::Flags::Flag0;
        functions->stream2_container_req_release(container, nullptr);
        if (status == SOS_DONE_ERROR)
            container->flags |= stream2_container::Flags::Flag6;

        container->flags &= (stream2_container::Flags)65527; //TODO: Simplify this. Likely zero's some lower bits
        *globalState->g_pStream2ContainerToLoad = nullptr;
        return false;
    }
    else if(*globalState->g_stream2ContainerLoaded) //Container loaded, clear g_pStream2ContainerToLoad
    {
        *globalState->g_pStream2ContainerToLoad = nullptr;
    }

    return true;
}

//Original name in exe is thread_dummy
FunHook<int __cdecl(stream2_os_device* device)> stream2_os_device_thread_hook //0x000ADFD0
{
    0x000ADFD0,
    [](stream2_os_device* device) -> int
    {
        static RSL2_GlobalState* globalState = GetGlobalState();
        static rfg::RfgFunctions* functions = GetRfgFunctions();

        bool result = true;
        while (result)
        {
            stream2_os_request* curRequest = device->request_pool + device->q;
            result = functions->stream2_os_device_process(device, nullptr);
        }

        return 0;
    }
};