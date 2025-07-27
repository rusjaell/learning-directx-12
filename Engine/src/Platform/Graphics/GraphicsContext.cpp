#include "GraphicsContext.h"

#include <stdexcept>
#include <memory>
#include "Application.h"

#include "Utilities/Utils.h"
#include "Utilities/Logger.h"

GraphicsContext::GraphicsContext(const std::shared_ptr<Window>& window)
    : _window(window), _canvasWidth(window->GetWidth()), _canvasHeight(window->GetWidth())
{
    _debug.EnableDebug();

    UINT factoryFlags = 0;
#if defined(_DEBUG)
    factoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif

    if (FAILED(CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&_factory)))) {
        throw std::runtime_error("Failed to create DXGI Factory");
    }

    if (FAILED(_factory->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE::DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&_adapter)))) {
        throw std::runtime_error("Failed to get graphics adapter");
    }

    DXGI_ADAPTER_DESC desc;
    _adapter->GetDesc(&desc);

    double vramInGB = static_cast<double>(desc.DedicatedVideoMemory) / 1024 / 1024 / 1024;
    double sharedRamInGB = static_cast<double>(desc.SharedSystemMemory) / 1024 / 1024 / 1024;

    Logger::Debug("Adapter Info:");
    Logger::Debug("- Description: {}", Utils::ToNarrowString(desc.Description));
    Logger::Debug("- Vendor ID: {}", desc.VendorId);
    Logger::Debug("- Device ID: {}", desc.DeviceId);
    Logger::Debug("- SubSys ID: {}", desc.SubSysId);
    Logger::Debug("- Revision: {}", desc.Revision);
    Logger::Debug("- VRAM: {:.2f} GB", vramInGB);
    Logger::Debug("- Shared RAM: {:.2f} GB", sharedRamInGB);

    if (FAILED(D3D12CreateDevice(_adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), &_device))) {
        throw std::runtime_error("Failed to create D3D12 device");
    }

    D3D12_COMMAND_QUEUE_DESC commandQueueDesc =
    {
        .Type = D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT,
        .Priority = D3D12_COMMAND_QUEUE_PRIORITY::D3D12_COMMAND_QUEUE_PRIORITY_HIGH,
        .Flags = D3D12_COMMAND_QUEUE_FLAGS::D3D12_COMMAND_QUEUE_FLAG_NONE,
        .NodeMask = 0
    };

    if (FAILED(_device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&_directQueue)))) {
        throw std::runtime_error("Failed to create command queue");
    }

    if (FAILED(_device->CreateFence(_directQueueFenceValue, D3D12_FENCE_FLAGS::D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_directQueueFence)))) {
        throw std::runtime_error("Failed to create fence");
    }

    _directQueueFenceEvent = CreateEvent(nullptr, false, false, nullptr);
    if (_directQueueFenceEvent == nullptr) {
        throw std::runtime_error("Failed to create fence event");
    }

    DXGI_SWAP_CHAIN_DESC1 swapChainDesc =
    {
        .Width = _canvasWidth,
        .Height = _canvasHeight,
        .Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM,
        .Stereo = false,
        .SampleDesc =
        {
            .Count = 1,
            .Quality = 0
        },
        .BufferUsage = DXGI_USAGE_BACK_BUFFER,// | DXGI_USAGE_RENDER_TARGET_OUTPUT,
        .BufferCount = BUFFER_COUNT,
        .Scaling = DXGI_SCALING::DXGI_SCALING_STRETCH,
        .SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_FLIP_DISCARD,
        .AlphaMode = DXGI_ALPHA_MODE::DXGI_ALPHA_MODE_IGNORE,
        .Flags = DXGI_SWAP_CHAIN_FLAG::DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG::DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING,
    };

    DXGI_SWAP_CHAIN_FULLSCREEN_DESC swapChainFullscreenDesc =
    {
        .RefreshRate =
        {
            .Numerator = 0,
            .Denominator = 0,
        },
        .ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER::DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED,
        .Scaling = DXGI_MODE_SCALING::DXGI_MODE_SCALING_UNSPECIFIED,
        .Windowed = true,
    };

    Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain1;
    if (FAILED(_factory->CreateSwapChainForHwnd(_directQueue.Get(), window->GetHandle(), &swapChainDesc, &swapChainFullscreenDesc, nullptr, &swapChain1))) {
        throw std::runtime_error("Failed to create swap chain");
    }

    if (FAILED(swapChain1.As(&_swapChain))) {
        throw std::runtime_error("Failed to query swap chain interface");
    }

    D3D12_DESCRIPTOR_HEAP_DESC heapDescriptor =
    {
        .Type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
        .NumDescriptors = BUFFER_COUNT,
        .Flags = D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
        .NodeMask = 0,
    };

    if (FAILED(_device->CreateDescriptorHeap(&heapDescriptor, IID_PPV_ARGS(&_rtvDescriptorHeap)))) {
        throw std::runtime_error("Failed to create descriptor heap");
    }

    D3D12_CPU_DESCRIPTOR_HANDLE rtvHeapStartHandle = _rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
    _rtvHeapIncrement = _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    for (int i = 0; i < BUFFER_COUNT; i++) {
        _rtvCPUDescriptorHandles[i] = {
            .ptr = rtvHeapStartHandle.ptr + i * _rtvHeapIncrement
        };
    }

    GetBuffers();

    if (FAILED(_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_commandAllocator)))) {
        throw std::runtime_error("Failed to create command allocator");
    }

    if (FAILED(_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT, _commandAllocator.Get(), nullptr, IID_PPV_ARGS(&_commandList)))) {
        throw std::runtime_error("Failed to create command list");
    }

    SetupViewportAndRect();
}

GraphicsContext::~GraphicsContext()
{
    FlushQueue(BUFFER_COUNT);

    _commandList.Reset();
    _commandAllocator.Reset();
    _rtvDescriptorHeap.Reset();

    ReleaseBuffers();

    _swapChain.Reset();

    CloseHandle(_directQueueFenceEvent);
    
    _directQueue.Reset();
    _directQueueFence.Reset();
    _device.Reset();

    _factory.Reset();

    _adapter.Reset();

    _debug.DisableDebug();
}

ID3D12GraphicsCommandList7* GraphicsContext::SetupAndGetCommandList()
{
    PrepareCommandList();
    return _commandList.Get();
}

void GraphicsContext::DrawCommandList()
{
    _commandList->Close();

    ID3D12CommandList* lists[] = { _commandList.Get()};
    _directQueue->ExecuteCommandLists(1, lists);

    FlushQueue();

    _commandAllocator->Reset();
    _commandList->Reset(_commandAllocator.Get(), nullptr);
}

void GraphicsContext::WaitForFence(int fenceValue)
{
    if (fenceValue == -1) {
        fenceValue = _directQueueFenceValue;
    }

    if (_directQueueFence->GetCompletedValue() < fenceValue)
    {
        if (SUCCEEDED(_directQueueFence->SetEventOnCompletion(fenceValue, _directQueueFenceEvent))) {
            if (WaitForSingleObject(_directQueueFenceEvent, 30000) == WAIT_OBJECT_0) {
                return;
            }
        }

        while (_directQueueFence->GetCompletedValue() < fenceValue);
        /*{
            Sleep(1);
        }*/
    }
}

void GraphicsContext::FlushQueue(int flushCount)
{
    for (int i = 0; i < flushCount; i++) {
        _directQueue->Signal(_directQueueFence.Get(), ++_directQueueFenceValue);
        WaitForFence();
    }
}

void GraphicsContext::BeginFrame()
{
    UINT width = _window->GetWidth();
    UINT height = _window->GetHeight();

    if (_canvasWidth != width || _canvasHeight != height) 
    {
        FlushQueue(BUFFER_COUNT);

        ReleaseBuffers();
        _swapChain->ResizeBuffers(BUFFER_COUNT, width, height, DXGI_FORMAT::DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG::DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG::DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING);
        GetBuffers();

        _canvasWidth = width;
        _canvasHeight = height;

        SetupViewportAndRect();
    }

    _currentBuffer = _swapChain->GetCurrentBackBufferIndex();

    D3D12_RESOURCE_BARRIER rtvSetupBarrier =
    {
       .Type = D3D12_RESOURCE_BARRIER_TYPE::D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
       .Flags = D3D12_RESOURCE_BARRIER_FLAGS::D3D12_RESOURCE_BARRIER_FLAG_NONE,
       .Transition =
        {
            .pResource = _buffers[_currentBuffer].Get(),
            .Subresource = 0,
            .StateBefore = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_PRESENT,
            .StateAfter = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_RENDER_TARGET,
        }
    };

    ID3D12GraphicsCommandList7* commandList = _commandList.Get();
    
    constexpr float clearColor[] = { 0.392f, 0.584f, 0.929f, 1.0f };

    commandList->ResourceBarrier(1, &rtvSetupBarrier);
    commandList->ClearRenderTargetView(_rtvCPUDescriptorHandles[_currentBuffer], clearColor, 0, nullptr);
}

void GraphicsContext::EndFrame()
{
    ID3D12GraphicsCommandList7* commandList = _commandList.Get();

    D3D12_RESOURCE_BARRIER barrier =
    {
       .Type = D3D12_RESOURCE_BARRIER_TYPE::D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
       .Flags = D3D12_RESOURCE_BARRIER_FLAGS::D3D12_RESOURCE_BARRIER_FLAG_NONE,
       .Transition =
        {
            .pResource = _buffers[_currentBuffer].Get(),
            .Subresource = 0,
            .StateBefore = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_RENDER_TARGET,
            .StateAfter = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_PRESENT,
        }
    };

    commandList->ResourceBarrier(1, &barrier);

    DrawCommandList();

    _swapChain->Present(0, 0);
}

void GraphicsContext::PrepareCommandList()
{
    _currentBuffer = _swapChain->GetCurrentBackBufferIndex();

    _commandList->RSSetViewports(1, &_viewport);
    _commandList->RSSetScissorRects(1, &_fullRect);
    _commandList->OMSetRenderTargets(1, &_rtvCPUDescriptorHandles[_currentBuffer], false, nullptr);
}

void GraphicsContext::GetBuffers()
{
    for (int i = 0; i < BUFFER_COUNT; i++) {
        _swapChain->GetBuffer(i, IID_PPV_ARGS(&_buffers[i]));
        _device->CreateRenderTargetView(_buffers[i].Get(), nullptr, _rtvCPUDescriptorHandles[i]);
        //D3D12_RENDER_TARGET_VIEW_DESC rtvDescriptor =
        //{
        //    .Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM,
        //    .ViewDimension = D3D12_RTV_DIMENSION::D3D12_RTV_DIMENSION_TEXTURE2D,
        //    .Texture2D = {
        //        .MipSlice = 0,
        //        .PlaneSlice = 0
        //    },
        //};
        //_device->CreateRenderTargetView(_buffers[i].Get(), &rtvDescriptor, _rtvCPUDescriptorHandles[i]);
    }
}

void GraphicsContext::ReleaseBuffers()
{
    for (int i = 0; i < BUFFER_COUNT; i++) {
        _buffers[i].Reset();
    }
}

void GraphicsContext::SetupViewportAndRect()
{
    _viewport =
    {
        .TopLeftX = 0.0f,
        .TopLeftY = 0.0f,
        .Width = static_cast<FLOAT>(_canvasWidth),
        .Height = static_cast<FLOAT>(_canvasHeight),
        .MinDepth = 1.0f,
        .MaxDepth = 0.0f,
    };

    _fullRect =
    {
        .left = 0,
        .top = 0,
        .right = static_cast<LONG>(_canvasWidth),
        .bottom = static_cast<LONG>(_canvasHeight),
    };
}
