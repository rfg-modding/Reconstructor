#include "ChunkLoading.h"

void __stdcall ChunkBaseLoadInternalHookFunc(uchar* gpu_data, const char* filename, uchar* cpu_data, int cpu_data_size, int gpu_data_size, c_chunk_base* chunk_base, bool init_render_data)
{
    auto a = 2;
    ChunkBaseLoadInternalHook.CallTarget(gpu_data, filename, cpu_data, cpu_data_size, gpu_data_size, chunk_base, init_render_data);
    auto b = 2;
}
FunHook<ChunkBaseLoadInternal_Type> ChunkBaseLoadInternalHook { 0x0, ChunkBaseLoadInternalHookFunc };