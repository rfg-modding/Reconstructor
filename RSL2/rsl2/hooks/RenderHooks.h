#pragma once
#include "common/patching/FunHook.h"
#include "common/windows/WindowsWrapper.h"
#include <d3d11.h>

struct IDXGISwapChain;
namespace keen
{
    struct GraphicsSystem;
    struct RenderSwapChain;
}

using D3D11_ResizeBuffersHook_Type = HRESULT __stdcall(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);
extern HRESULT __stdcall D3D11_ResizeBuffersHookFunc(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);
extern FunHook<D3D11_ResizeBuffersHook_Type> D3D11_ResizeBuffersHook;

//keen::GraphicsCommandBuffer *__cdecl keen::graphics::beginFrame(keen::GraphicsSystem *pGraphicsSystem, keen::RenderSwapChain *pSwapChain)
extern FunHook<void* (keen::GraphicsSystem* pGraphicsSystem, keen::RenderSwapChain* pSwapChain)> keen_graphics_beginFrame;

using D3D11_PresentHook_Type = HRESULT __stdcall(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
extern HRESULT __stdcall D3D11_PresentHookFunc(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
extern FunHook<D3D11_PresentHook_Type> D3D11_PresentHook;

extern bool ReadyForD3D11Init();