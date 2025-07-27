#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define UNICODE

#include <windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <wrl.h>
#include <stdexcept>
#include <memory>
#include <iostream>

#include "Application.h"

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    if (!Application::Get().Initialize(hInstance, hPrevInstance, lpCmdLine, nCmdShow)) {
        return -1;
    }
    Application::Get().Run();
    return 0;
}