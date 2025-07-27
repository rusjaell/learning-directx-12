#include "Application.h"

#include <utility>

#include "Utilities/Utils.h"
#include "Utilities/Logger.h"

bool Application::Initialize(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    _handle = hInstance;
    
    Logger::Info("Attaching Console...");
    Utils::AttachConsole();
    try
    {
        Logger::Info("Creating window...");
        _window = std::make_shared<Window>(hInstance);
        _window->Show(nCmdShow);
        _window->SetTitle(L"DirectX 12 Test Environment");
    }
    catch (const std::exception& e) {
        Logger::Error("Window Initialization Error: {}", e.what());
        MessageBoxA(nullptr, e.what(), "Window Initialization Error", MB_ICONERROR);
        std::exit(-1);
    }

    try 
    {
        Logger::Info("Initializing D3D12 context...");
        _graphicsContext = std::make_shared<GraphicsContext>(_window);
    }
    catch (const std::exception& e) {
        Logger::Error("GraphicsContext Initialization Error: {}", e.what());
        MessageBoxA(nullptr, e.what(), "GraphicsContext Initialization Error", MB_ICONERROR);
        std::exit(-1);
    }

    Logger::Info("Creating FrameTime...");
    _frameTime = std::make_unique<FrameTime>();

    Logger::Info("Creating Layer Stack...");
    _layerStack = std::make_unique<LayerStack>();

    Logger::Info("Creating Layers...");
    CreateLayers();
    return true;
}

void Application::Run()
{
    Logger::Info("Application Running");

    //_window->SetFullScreen(true);
    while (!_window->ShouldClose())
    {
        _frameTime->Tick();

        _window->PollEvents();
        _layerStack->UpdateLayers(_frameTime);

        _graphicsContext->BeginFrame();
        _layerStack->DrawLayers();
        _graphicsContext->EndFrame();
    }

    Logger::Info("Clearing Layers");
    _layerStack->ClearLayers();

    Logger::Info("Application Finished Running");
}

void Application::CreateLayers()
{
    Logger::Info("Creating Demo Layer");
    _demoLayer = std::make_shared<DemoLayer>();
    _demoLayer->SetLayerIndex(0);
    _layerStack->AttachLayer(_demoLayer);

    Logger::Info("Creating IMGUI Layer");
    _imGUILayer = std::make_shared<IMGUILayer>(_window, _graphicsContext);
    _imGUILayer->SetLayerIndex(1);
    _layerStack->AttachLayer(_imGUILayer);
}