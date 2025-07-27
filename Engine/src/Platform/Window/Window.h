#pragma once 

#include <windows.h>
#include <string>

#include <Memory>

class Window
{
public:
    Window(HINSTANCE hInstance);
    ~Window();

    void PollEvents() const;

    HWND GetHandle() const { return _hWindow; }
    UINT GetWidth() const { return _width; }
    UINT GetHeight() const { return _height; }
    bool IsFullscreen() const { return _isFullScreen; }
    bool ShouldClose() {
        bool shouldClose = _shouldClose; 
        _shouldClose = false; 
        return shouldClose; 
    }

    void SetTitle(const std::wstring& text);

    void SetFullScreen(bool fullscreen);
    void Show(int nCmdShow);

private:
    static LRESULT CALLBACK WindowProcSetup(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK WindowProcRedirect(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    LRESULT WindowProcessMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:

    bool _shouldClose = false;
    bool _isFullScreen = false;

    UINT _width = 1280;
    UINT _height = 720;
    std::wstring _title = L"Window";

    HWND _hWindow = nullptr;
    ATOM _windowClass = 0;

    HINSTANCE _hInstance = nullptr;
};
