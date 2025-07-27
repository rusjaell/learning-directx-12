#pragma once

#include <Memory.h>
#include <Windows.h>

#include "Platform/Window/Window.h"
#include "Platform/Graphics/GraphicsContext.h"
#include "Utilities/FrameTime.h"

#include "Layers/LayerStack.h"
#include "Layers/DemoLayer.h"
#include "Layers/IMGUILayer.h"

class Application 
{
private:
	Application() = default;
	~Application() = default;
	Application(const Application&) = delete;
	Application& operator=(const Application&) = delete;

public:
	static Application& Get()
	{
		static Application instance;
		return instance;
	}

	HINSTANCE GetHandle() const { return _handle; }
	std::shared_ptr<Window> GetWindow() const { return _window; }
	std::shared_ptr<GraphicsContext> GetGraphicsContext() const { return _graphicsContext; }
	std::shared_ptr<DemoLayer> GetDemoLayer() const { return _demoLayer; }
	std::shared_ptr<IMGUILayer> GetIMGUILayer() const { return _imGUILayer; }
	LayerStack* GetLayerStack() const { return _layerStack.get(); }
	const FrameTime* GetFrameTime() const { return _frameTime.get(); }

	bool Initialize(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow);
	void Run();

private:
	void CreateLayers();

private:
	HINSTANCE _handle = {} ;
	std::shared_ptr<Window> _window;
	std::shared_ptr<GraphicsContext> _graphicsContext;
	std::shared_ptr<DemoLayer> _demoLayer;
	std::shared_ptr<IMGUILayer> _imGUILayer;
	std::unique_ptr<LayerStack> _layerStack;
	std::shared_ptr<FrameTime> _frameTime;
};