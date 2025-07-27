#pragma once

#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <string>
#include <memory>
#include "GraphicsDebug.h"

class Window;
class GraphicsContext
{
public:
    static const UINT BUFFER_COUNT = 2;

public:
    GraphicsContext(const std::shared_ptr<Window>& window);
    ~GraphicsContext();

    ID3D12GraphicsCommandList7* SetupAndGetCommandList();
    void DrawCommandList();

    void WaitForFence(int fenceValue = -1);
    void FlushQueue(int flushCount = 1);

    IDXGIFactory7* GetFactory() const { return _factory.Get(); }
    ID3D12Device* GetDevice() const { return _device.Get(); }
    ID3D12CommandQueue* GetCommandQueue() const { return _directQueue.Get(); }
    IDXGISwapChain3* GetSwapChain() const { return _swapChain.Get(); }

    void BeginFrame();
    void EndFrame();

private:
    Microsoft::WRL::ComPtr<IDXGIAdapter> GetAdapter();

    void PrepareCommandList();
    void GetBuffers();
    void ReleaseBuffers();
    void SetupViewportAndRect();

private:
    std::shared_ptr<Window> _window;
    D3D12_VIEWPORT _viewport;
    D3D12_RECT _fullRect;

    UINT _canvasWidth;
    UINT _canvasHeight;
    GraphicsDebug _debug;

    Microsoft::WRL::ComPtr<IDXGIFactory7> _factory;
    Microsoft::WRL::ComPtr<ID3D12Device> _device;
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> _directQueue;
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> _commandAllocator;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList7> _commandList;

    Microsoft::WRL::ComPtr<ID3D12Fence1> _directQueueFence;
    UINT64 _directQueueFenceValue = 0;
    HANDLE _directQueueFenceEvent = nullptr;

    Microsoft::WRL::ComPtr<IDXGISwapChain3> _swapChain;
    Microsoft::WRL::ComPtr<ID3D12Resource2> _buffers[BUFFER_COUNT];
    D3D12_CPU_DESCRIPTOR_HANDLE _rtvCPUDescriptorHandles[BUFFER_COUNT] = {};
    UINT _rtvHeapIncrement = 0;
    UINT _currentBuffer = 0;

    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> _rtvDescriptorHeap;

    Microsoft::WRL::ComPtr<IDXGIAdapter1> _adapter;
};