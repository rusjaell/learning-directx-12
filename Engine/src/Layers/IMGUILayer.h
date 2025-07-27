#pragma once

#include <wrl.h>
#include <d3d12.h>
#include <memory>

#include "BaseLayer.h"

class Window;
class GraphicsContext;
class IMGUILayer : public BaseLayer
{
public:
	IMGUILayer(const std::shared_ptr<Window>& window, const std::shared_ptr<GraphicsContext>& graphicsContext);
	~IMGUILayer();

	void Update(const std::shared_ptr<FrameTime>& frameTime) override;
	void Draw() override;

private:
	std::shared_ptr<Window> _window;
	std::shared_ptr<GraphicsContext> _graphicsContext;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> _descriptorHeap;
};
