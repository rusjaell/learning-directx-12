#include "Utils.h"

#include <Windows.h>
#include <iostream>
#include <string>
#include <locale>
#include <codecvt>

namespace Utils
{
    void AttachConsole()
    {
        AllocConsole();

        FILE* fp;
        freopen_s(&fp, "CONOUT$", "w", stdout);
        freopen_s(&fp, "CONOUT$", "w", stderr);
        freopen_s(&fp, "CONIN$", "r", stdin);

        std::ios::sync_with_stdio(true);

        std::cout.clear();
        std::cerr.clear();

        std::cout.flush();
        std::cerr.flush();
    }

    std::string ToNarrowString(const std::wstring& wstr)
    {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
        return conv.to_bytes(wstr);
    }
}