#include "GraphicsDebug.h"

void GraphicsDebug::EnableDebug()
{
#if defined(_DEBUG)
	DXGIGetDebugInterface1(0, IID_PPV_ARGS(&_debugDXGI));
	D3D12GetDebugInterface(IID_PPV_ARGS(&_debugD3D));
	_debugD3D->EnableDebugLayer();
#endif
}

void GraphicsDebug::DisableDebug()
{
#if defined(_DEBUG)
	_debugDXGI->ReportLiveObjects(DXGI_DEBUG_ALL, (DXGI_DEBUG_RLO_FLAGS)(DXGI_DEBUG_RLO_IGNORE_INTERNAL | DXGI_DEBUG_RLO_DETAIL));
	_debugDXGI.Reset();
	_debugD3D.Reset();
#endif
}
