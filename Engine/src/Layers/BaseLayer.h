#pragma once

#include "Utilities/FrameTime.h"

class BaseLayer
{
public:
	BaseLayer() = default;
	virtual ~BaseLayer() = default;

	int GetLayerIndex() const { return _layerIndex; }
	void SetLayerIndex(int index) { _layerIndex = index; }

	virtual void OnAttach() {};
	virtual void Update(const std::shared_ptr<FrameTime>& frameTime) = 0;
	virtual void Draw() = 0;
	virtual void OnDettach() {};

private:
	int _layerIndex = 0;
};

