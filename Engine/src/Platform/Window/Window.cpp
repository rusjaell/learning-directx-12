#include "Window.h"

#include "Platform/Graphics/GraphicsContext.h"

#include <imgui_impl_win32.h>
#include <stdexcept>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

Window::Window(HINSTANCE hInstance)
{
    _hInstance = hInstance;

    WNDCLASSEX windowClassEx =
    {
        .cbSize = sizeof(WNDCLASSEX),
        .style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW,
        .lpfnWndProc = WindowProcSetup,
        .cbClsExtra = 0,
        .cbWndExtra = 0,
        .hInstance = _hInstance,// GetModuleHandle(nullptr),
        .hIcon = LoadIcon(nullptr, IDI_APPLICATION),
        .hCursor = LoadCursor(nullptr, IDC_ARROW),
        .hbrBackground = nullptr,
        .lpszMenuName = nullptr,
        .lpszClassName = L"BasicD3DWindowClass",
        .hIconSm = LoadIcon(nullptr, IDI_APPLICATION),
    };

    _windowClass = RegisterClassEx(&windowClassEx);
    if (!_windowClass) {
        throw std::runtime_error("Failed to register window class");
    }

    //POINT pos = {};
    //GetCursorPos(&pos);

    //HMONITOR monitor = MonitorFromPoint(pos, MONITOR_DEFAULTTOPRIMARY);

    //MONITORINFO monitorInfo = {
    //    .cbSize = sizeof(MONITORINFO),
    //};
    //GetMonitorInfoW(monitor, &monitorInfo);

    RECT windowRect = { 0, 0, static_cast<LONG>(_width), static_cast<LONG>(_height) };
    AdjustWindowRect(&windowRect, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU, FALSE);

    _hWindow = CreateWindowEx(
        WS_EX_OVERLAPPEDWINDOW | WS_EX_APPWINDOW,
        (LPCWSTR)_windowClass,
        _title.c_str(),
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        //monitorInfo.rcWork.left,
        //monitorInfo.rcWork.top,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        windowRect.right - windowRect.left,
        windowRect.bottom - windowRect.top,
        //monitorInfo.rcWork.right - monitorInfo.rcWork.left,
        //monitorInfo.rcWork.bottom - monitorInfo.rcWork.top,
        nullptr,
        nullptr,
        _hInstance,
        this);

    if (_hWindow == nullptr) {
        throw std::runtime_error("Failed to create window");
    }

    GetWindowDpiAwarenessContext(_hWindow);
}

Window::~Window()
{
    if (_hWindow != nullptr) {
        DestroyWindow(_hWindow);
    }

    UnregisterClass((LPCWSTR)_windowClass, _hInstance);
}

void Window::SetTitle(const std::wstring& text)
{
    _title = text;
    SetWindowText(_hWindow, text.c_str());
}

void Window::SetFullScreen(bool fullscreen)
{
    DWORD style = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
    DWORD exStyle = WS_EX_OVERLAPPEDWINDOW | WS_EX_APPWINDOW;

    if(fullscreen) {
        style = WS_POPUP | WS_VISIBLE;
        exStyle = WS_EX_APPWINDOW;
    }

    SetWindowLongPtr(_hWindow, GWL_STYLE, style);
    SetWindowLongPtr(_hWindow, GWL_EXSTYLE, exStyle);
    
    if (fullscreen) {

        HMONITOR monitor = MonitorFromWindow(_hWindow, MONITOR_DEFAULTTONEAREST);

        MONITORINFO monitorInfo = {
            .cbSize = sizeof(MONITORINFO),
        };

        if (GetMonitorInfoW(monitor, &monitorInfo))
        {
            LONG monX = monitorInfo.rcMonitor.left;
            LONG monY = monitorInfo.rcMonitor.top;
            LONG monWidth = monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;
            LONG monHeight = monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;
            SetWindowPos(_hWindow, nullptr, monX, monY, monWidth, monHeight, SWP_NOOWNERZORDER | SWP_FRAMECHANGED | SWP_SHOWWINDOW);
        }
    }
    else {
        ShowWindow(_hWindow, SW_MAXIMIZE);
    }

    _isFullScreen = fullscreen;
}

void Window::PollEvents() const
{
    MSG msg = {};
    while (PeekMessage(&msg, _hWindow, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void Window::Show(int nCmdShow)
{
    ShowWindow(_hWindow, nCmdShow);
    UpdateWindow(_hWindow);
}

LRESULT CALLBACK Window::WindowProcSetup(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_NCCREATE)
    {
        CREATESTRUCT* createStruct = reinterpret_cast<CREATESTRUCT*>(lParam);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(createStruct->lpCreateParams));
        SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::WindowProcRedirect));
        return WindowProcRedirect(hwnd, msg, wParam, lParam); 
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}


LRESULT CALLBACK Window::WindowProcRedirect(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    Window* window = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    return window->WindowProcessMessage(hwnd, msg, wParam, lParam);
}

LRESULT Window::WindowProcessMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam)) {
        return true;
    }

    switch (msg)
    {
    case WM_KEYDOWN:
        if (wParam == VK_F11) {
            SetFullScreen(!_isFullScreen);
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_SIZE:
        if (lParam && (HIWORD(lParam) != _height || LOWORD(lParam) != _width)) {
            _width = LOWORD(lParam);
            _height = HIWORD(lParam);
        }
        return 0;

    case WM_CLOSE:
        _shouldClose = true;
        return 0;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}