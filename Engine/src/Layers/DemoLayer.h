#pragma once

#include "BaseLayer.h"

class DemoLayer : public BaseLayer
{
public:
	void Update(const std::shared_ptr<FrameTime>& frameTime) override;
	void Draw() override;

private:
	
};

