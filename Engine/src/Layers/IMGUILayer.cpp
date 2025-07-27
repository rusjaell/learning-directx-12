#include "IMGUILayer.h"
#include "Platform/Window/Window.h"
#include "Platform/Graphics/GraphicsContext.h"

#include <imgui.h>
#include <imgui_impl_dx12.h>
#include <imgui_impl_win32.h>

IMGUILayer::IMGUILayer(const std::shared_ptr<Window>& window, const std::shared_ptr<GraphicsContext>& graphicsContext)
{
    _window = window;
    _graphicsContext = graphicsContext;

    IMGUI_CHECKVERSION();

    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    unsigned char* pixels;
    int width, height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

    ImGui_ImplWin32_Init(window->GetHandle());

    D3D12_DESCRIPTOR_HEAP_DESC imguiDescHeapDesc = {};
    imguiDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    imguiDescHeapDesc.NumDescriptors = 1;
    imguiDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    imguiDescHeapDesc.NodeMask = 0;

    graphicsContext->GetDevice()->CreateDescriptorHeap(&imguiDescHeapDesc, IID_PPV_ARGS(&_descriptorHeap));

    // Initialize ImGui DirectX12 backend
    ImGui_ImplDX12_Init(
        graphicsContext->GetDevice(),
        GraphicsContext::BUFFER_COUNT,
        DXGI_FORMAT_R8G8B8A8_UNORM,
        _descriptorHeap.Get(),
        _descriptorHeap->GetCPUDescriptorHandleForHeapStart(),
        _descriptorHeap->GetGPUDescriptorHandleForHeapStart()
    );
}

IMGUILayer::~IMGUILayer()
{  
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    _descriptorHeap.Reset();
}

void IMGUILayer::Update(const std::shared_ptr<FrameTime>& frameTime)
{
    ImGui_ImplWin32_NewFrame();
    ImGui_ImplDX12_NewFrame();
    ImGui::NewFrame();
}

void IMGUILayer::Draw()
{
    ImGui::Render();

    ID3D12GraphicsCommandList* commandList = _graphicsContext->SetupAndGetCommandList();

    ID3D12DescriptorHeap* heaps[] = { _descriptorHeap.Get() };
    commandList->SetDescriptorHeaps(1, heaps);

    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);

    _graphicsContext->DrawCommandList();
}
