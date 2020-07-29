#pragma once

#include <d3d11_1.h>
#include <DXGI.h>
#include <d3d11.h>
#include <d3dcompiler.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

namespace keen
{
    //Todo: Move structures used in multiples classes to their own files. All here for now for convenience
    namespace graphics
    {
        enum WindowMode
        {
            WindowMode_Windowed = 0x0,
            WindowMode_Borderless = 0x1,
            WindowMode_Fullscreen = 0x2,
            WindowMode_Count = 0x3,
        };
    }

    enum PrimitiveType
    {
        PrimitiveType_Invalid = 0xFFFFFFFF,
        PrimitiveType_TriangleList = 0x0,
        PrimitiveType_TriangleStrip = 0x1,
        PrimitiveType_LineList = 0x2,
        PrimitiveType_LineStrip = 0x3,
        PrimitiveType_QuadList = 0x4,
        PrimitiveType_Count = 0x5,
    };

    enum PixelFormat
    {
        PixelFormat_None = 0x0,
        PixelFormat_Bc1 = 0x1,
        PixelFormat_Bc1_Gamma = 0x2,
        PixelFormat_Bc2 = 0x3,
        PixelFormat_Bc2_Gamma = 0x4,
        PixelFormat_Bc3 = 0x5,
        PixelFormat_Bc3_Gamma = 0x6,
        PixelFormat_Bc4 = 0x7,
        PixelFormat_Bc4_Gamma = 0x8,
        PixelFormat_Bc5 = 0x9,
        PixelFormat_Bc5_Gamma = 0xA,
        PixelFormat_Bc6H_SF16 = 0xB,
        PixelFormat_Bc6H_UF16 = 0xC,
        PixelFormat_Bc7 = 0xD,
        PixelFormat_Bc7_Gamma = 0xE,
        PixelFormat_Index4 = 0xF,
        PixelFormat_A4 = 0x10,
        PixelFormat_L4 = 0x11,
        PixelFormat_A8 = 0x12,
        PixelFormat_L8 = 0x13,
        PixelFormat_Index8 = 0x14,
        PixelFormat_L4A4 = 0x15,
        PixelFormat_G8R8 = 0x16,
        PixelFormat_R8G8 = 0x17,
        PixelFormat_R5G6B5 = 0x18,
        PixelFormat_R5G5B5A1 = 0x19,
        PixelFormat_R4G4B4A4 = 0x1A,
        PixelFormat_R5G5B5A3 = 0x1B,
        PixelFormat_Index14x2 = 0x1C,
        PixelFormat_R8G8B8 = 0x1D,
        PixelFormat_R8G8B8_Gamma = 0x1E,
        PixelFormat_R32F = 0x1F,
        PixelFormat_Y16X16 = 0x20,
        PixelFormat_A8R8G8B8 = 0x21,
        PixelFormat_A8R8G8B8_Gamma = 0x22,
        PixelFormat_X8R8G8B8 = 0x23,
        PixelFormat_X8R8G8B8_Gamma = 0x24,
        PixelFormat_A8B8G8R8 = 0x25,
        PixelFormat_A8B8G8R8_Gamma = 0x26,
        PixelFormat_R8G8B8A8 = 0x27,
        PixelFormat_R8G8B8A8_Gamma = 0x28,
        PixelFormat_A2R10G10B10F = 0x29,
        PixelFormat_A16B16G16R16F = 0x2A,
        PixelFormat_A32B32G32R32F = 0x2B,
        PixelFormat_Depth8 = 0x2C,
        PixelFormat_Depth16 = 0x2D,
        PixelFormat_Depth24Stencil8 = 0x2E,
        PixelFormat_ETC1_RGB8 = 0x2F,
        PixelFormat_ETC1_RGB8A4 = 0x30,
        PixelFormat_Pvrtc2 = 0x31,
        PixelFormat_Pvrtc4 = 0x32,
        PixelFormat_Pvrtc2A = 0x33,
        PixelFormat_Pvrtc4A = 0x34,
        PixelFormat_Ctx1 = 0x35,
        PixelFormat_DxN = 0x36,
        PixelFormat_Depth32F = 0x37,
        PixelFormat_Depth24FStencil8 = 0x38,
        PixelFormat_R11G11B10F = 0x39,
        PixelFormat_Depth32FStencil8 = 0x3A,
        PixelFormat_Depth16Stencil8 = 0x3B,
        PixelFormat_Count = 0x3C,
        PixelFormat_Invalid = 0x3C,
        PixelFormat_Native_RGBA32 = 0x27,
        PixelFormat_Native_RGBA32_Gamma = 0x28,
        PixelFormat_Default_Depth = 0x2E,
        PixelFormat_Dxt1 = 0x1,
        PixelFormat_Dxt1_Gamma = 0x2,
        PixelFormat_Dxt23 = 0x3,
        PixelFormat_Dxt23_Gamma = 0x4,
        PixelFormat_Dxt45 = 0x5,
        PixelFormat_Dxt45_Gamma = 0x6,
    };

    struct GraphicsStateObject
    {
        unsigned int hash;
        unsigned int refCount;
    };

    struct VertexAttributeDescription
    {
        char id;
        char format;
        char inputStreamIndex;
        char instanceStepRate;
    };

    struct VertexFormat : keen::GraphicsStateObject
    {
        keen::VertexAttributeDescription attributes[17];
        unsigned int attributeOffsets[17];
        unsigned int attributeCount;
        unsigned int attributeIndices[17];
        unsigned int streamStride[6];
        unsigned int instanceDataStreamIndex;
    };

    struct TextureDescription
    {
        unsigned __int16 width;
        unsigned __int16 height;
        unsigned __int16 depth;
        unsigned __int16 flags;
        char type;
        char format;
        char lutFormat;
        char multiSampleType;
        char addressModeU;
        char addressModeV;
        char addressModeW;
        char levelCount;
        char cpuAccessMode;
        char gpuAccessMode;
    };

    struct TextureData
    {
        keen::TextureDescription description;
        ID3D11Resource* pTexture;
        ID3D11ShaderResourceView* pShaderView;
        DXGI_FORMAT d3dFormat;
    };

    //Todo: Make actual template for these
    struct Array__unsigned_char
    {
        char* m_pData;
        unsigned int m_size;
    };
    struct Array___D3D11_SIGNATURE_PARAMETER_DESC
    {
        _D3D11_SIGNATURE_PARAMETER_DESC* m_pData;
        unsigned int m_size;
    };

    struct VertexShader
    {
        //keen::Array<unsigned char> shaderCode;
        Array__unsigned_char shaderCode;
        ID3D11VertexShader* pVertexShader;
        //keen::Array<_D3D11_SIGNATURE_PARAMETER_DESC> inputSignature;
        Array___D3D11_SIGNATURE_PARAMETER_DESC inputSignature;
        unsigned int inputSignatureHash;
    };

    struct FragmentShader
    {
        ID3D11PixelShader* pPixelShader;
    };

    struct ShaderPipeline
    {
        keen::VertexShader* pVertexShader;
        keen::FragmentShader* pFragmentShader;
    };

    struct StockObjects
    {
        keen::ShaderPipeline m_shaderPipelines[3];
        keen::VertexShader* m_pStockVertexShaders[2];
        keen::FragmentShader* m_pStockFragmentShaders[3];
        keen::VertexFormat* m_pFormats[3];
        keen::TextureData m_textures[1];
    };

    struct BlendState : keen::GraphicsStateObject
    {
        ID3D11BlendState* pState;
    };

    struct DepthStencilState : keen::GraphicsStateObject
    {
        ID3D11DepthStencilState* pState;
    };

    struct RasterizerState : keen::GraphicsStateObject
    {
        ID3D11RasterizerState* pState;
    };

    struct SamplerState : keen::GraphicsStateObject
    {
        ID3D11SamplerState* pState;
    };

    const struct VertexInputBinding : keen::GraphicsStateObject
    {
        keen::VertexFormat* pVertexFormat;
        ID3D11InputLayout* pLayout;
        unsigned int geometryModeMask;
    };

    template <class T>
    class SizedArray
    {
        T* Data;
        unsigned int Size;
        unsigned int Capacity;
    };

    struct MemoryBlock
    {
        char* pStart;
        unsigned int size;
    };

    struct BasePoolAllocator //24 bytes
    {
        keen::MemoryBlock m_memoryBlock;
        unsigned int m_capacity;
        unsigned int m_size;
        unsigned int m_elementSize;
        unsigned int m_firstFreeIndex;
    };

    template <class T>
    class PoolAllocator //24 bytes
    {
        keen::BasePoolAllocator m_pool;
    };

    struct GraphicsStateObjectCache
    {
        char m_stateObjects[45];
        //keen::HashMap<unsigned int, keen::GraphicsStateObject *, keen::DefaultHashmapTraits<unsigned int, keen::GraphicsStateObject *> > m_stateObjects;
    };

    template <class T>
    class GraphicsStateObjectPool
    {
        keen::PoolAllocator<char> m_allocator;
        //keen::PoolAllocator<keen::VertexInputBinding> m_allocator;
        keen::GraphicsStateObjectCache m_cache;
        unsigned int m_peakSize;
        unsigned int m_cacheHits;
        unsigned int m_cacheRequests;
    };

    struct CombinedGraphicsStateObjectPool
    {
        keen::GraphicsStateObjectPool<keen::BlendState> m_blendStatePool;
        keen::GraphicsStateObjectPool<keen::RasterizerState> m_rasterizerStatePool;
        keen::GraphicsStateObjectPool<keen::SamplerState> m_samplerStatePool;
        keen::GraphicsStateObjectPool<keen::DepthStencilState> m_depthStencilStatePool;
        keen::GraphicsStateObjectPool<keen::VertexFormat> m_vertexFormatPool;
        keen::GraphicsStateObjectPool<keen::VertexInputBinding> m_vertexInputBindingPool;
    };

    struct DataBuffer
    {
        char* m_pCurrentPosition;
        char* m_pEnd;
        char* m_pBufferStart;
    };

    struct SkinningBatch
    {
        //keen::SkinningD3D11* pSkinningBuffer;
        void* pSkinningBuffer;

        char* pMappedBufferData;
    };

    struct SkinnedGeometryInstance
    {
        ID3D11Buffer* pVertexBuffer;
        char* pMappedData;
        unsigned int skinBufferOffset;
    };


    struct SkinningD3D11
    {
        ID3D11Buffer* m_pSkinningBuffer;
        keen::SkinningBatch m_skinningBatch;
        keen::SizedArray<keen::SkinnedGeometryInstance> m_instances[2];
        unsigned int m_currentFrameIndex;
        unsigned int m_currentBufferPosition;
        unsigned int m_size;
        unsigned int m_bufferSize;
        //keen::SoftwareSkinningJointMatrixData* m_pSkinningJointMatrices;
        void* m_pSkinningJointMatrices;
        
        unsigned int m_currentJointMatricesPosition;
        //keen::TaskQueue* m_pTaskQueue;
        void* m_pTaskQueue;

        keen::DataBuffer m_morphBuffer;
        unsigned int m_morphBufferSize;
        void* m_pMorphBufferData;
    };

    struct GraphicsCommandBuffer
    {
        ID3D11DeviceContext* pContext;
        ID3D11Buffer* pMappedConstantBuffer;
        //keen::RenderTarget* pCurrentRenderTarget;
        void* pCurrentRenderTarget;
        
        keen::SkinningD3D11* pSkinningBuffer;
        ID3D11Buffer* pImmediateVertexData;
        unsigned int immediateVertexBufferOffset;
        unsigned int immediateVertexBufferSize;
        unsigned int immediateVertexDataStride;
        unsigned int immediateVertexCount;
        keen::PrimitiveType immediatePrimitiveType;
        //keen::DownsampleDepthContext* pDownsampleDepthContext;
        void* pDownsampleDepthContext;
        
        keen::VertexFormat* pCurrentlyBoundVertexFormat;
        void* pRenderCommandBufferStorage;
        bool quadlistImmediateCommand;
        char* pCurrentImmediateBuffer;
        char quadBuffer[65536];
    };

    struct RenderTargetBuffer
    {
        keen::PixelFormat format;
        keen::TextureData* pDataBuffer;
    };

    struct RenderTarget
    {
        ID3D11RenderTargetView* renderTargetViews[8];
        ID3D11DepthStencilView* pDepthBufferView;
        keen::RenderTargetBuffer colorBuffers[8];
        keen::RenderTargetBuffer depthBuffer;
        unsigned int colorBufferCount;
        unsigned int width;
        unsigned int height;
    };


    struct GraphicsSystem;
    struct RenderSwapChain
    {
        keen::GraphicsSystem* pGraphicsSystem;
        keen::PixelFormat depthBufferFormat;
        HWND__* windowHandle;
        keen::TextureData backBufferColorData;
        keen::TextureData backBufferDepthData;
        keen::RenderTarget backBufferRenderTarget;
        keen::RenderTarget noGammaBackBufferRenderTarget;
        IDXGISwapChain* pSwapChain;
        DXGI_SWAP_CHAIN_DESC swapChainDescription;
        ID3D11RenderTargetView* pBackBufferRenderTargetView;
        ID3D11RenderTargetView* pNoGammaBackBufferRenderTargetView;
        ID3D11DepthStencilView* pBackBufferDepthView;
        unsigned int windowWidth;
        unsigned int windowHeight;
        unsigned int presentationInterval;
    };

    struct GraphicsSystemBase
    {
        //keen::ShaderFactory* pShaderFactory;
        //keen::MemoryAllocator* pSystemAllocator;
        void* pShaderFactory;
        void* pSystemAllocator;

        keen::CombinedGraphicsStateObjectPool stateObjectPool;
        keen::StockObjects stockObjects;
    };

    struct GraphicsSystem : keen::GraphicsSystemBase
    {
        ID3D11Device* pDevice;
        ID3D11DeviceContext* pImmediateContext;
        float screenAspectRatio;
        HWND__* createdWindowHandle;
        bool isWindowDestroyed;
        keen::SkinningD3D11 skinningBuffer;
        unsigned int ownerThreadId;
        unsigned int frontThreadId;
        keen::GraphicsCommandBuffer immediateCommandBuffer;
        keen::GraphicsCommandBuffer deferredCommandBuffer;
        keen::FragmentShader emptyFragmentShader;
        keen::RenderSwapChain* pDefaultSwapChain;
        keen::RenderSwapChain* pCurrentSwapChain;

        unsigned int currentFrameNumber;
        //keen::ScreenCapture* pScreenCapture;
        void* pScreenCapture;

        keen::graphics::WindowMode previousFullscreenMode;
        keen::graphics::WindowMode fullscreenMode;
        unsigned int exclusiveModeWidth;
        unsigned int exclusiveModeHeight;
        unsigned int exclusiveModeRefreshRateNumerator;
        unsigned int exclusiveModeRefreshRateDenominator;
        unsigned int windowModeWidth;
        unsigned int windowModeHeight;
    };
}