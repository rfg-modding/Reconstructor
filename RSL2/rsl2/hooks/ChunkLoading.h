#pragma once
#include "common/patching/FunHook.h"
#include "common/windows/WindowsWrapper.h"
#include "RFGR_Types/rfg/ChunkBase.h"
#include <d3d11.h>

using uchar = unsigned char;
using ChunkBaseLoadInternal_Type = void __stdcall(uchar* gpu_data, const char* filename, uchar* cpu_data, int cpu_data_size, int gpu_data_size, c_chunk_base* chunk_base, bool init_render_data);
extern void __stdcall ChunkBaseLoadInternalHookFunc(uchar* gpu_data, const char* filename, uchar* cpu_data, int cpu_data_size, int gpu_data_size, c_chunk_base* chunk_base, bool init_render_data);
extern FunHook<ChunkBaseLoadInternal_Type> ChunkBaseLoadInternalHook;