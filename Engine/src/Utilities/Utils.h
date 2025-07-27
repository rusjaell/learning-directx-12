#pragma once
#include <string>

namespace Utils
{
    void AttachConsole();
    std::string ToNarrowString(const std::wstring& wstr);
}