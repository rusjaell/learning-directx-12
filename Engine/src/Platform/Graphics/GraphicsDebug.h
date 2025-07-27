#pragma once

#ifdef _DEBUG
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <d3d12sdklayers.h>
#include <dxgidebug.h>
#endif

class GraphicsDebug
{
public:
	void EnableDebug();
	void DisableDebug();

#if defined(_DEBUG)
private:
	Microsoft::WRL::ComPtr<IDXGIDebug1> _debugDXGI;
	Microsoft::WRL::ComPtr<ID3D12Debug> _debugD3D;
#endif
};
