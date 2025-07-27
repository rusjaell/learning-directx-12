#pragma once

#include <iostream>
#include <chrono>
#include <iomanip>
#include <format>
#include <mutex>
#include <source_location>
#include <filesystem>

enum class LogLevel
{
    Info,
    Warning,
    Error,
    Debug
};

class Logger
{
public:
    template<typename... Args>
    static void Log(LogLevel level, std::format_string<Args...> fmt, Args&&... args)
    {
        const std::source_location& loc = std::source_location::current();

        std::string msg = std::format(fmt, std::forward<Args>(args)...);
        std::string output = DoFormatMessage(level, msg, loc);

        std::lock_guard lock(_mutex);
        std::cout << output << std::endl;
    }

    template<typename... Args>
    static void Info(std::format_string<Args...> fmt, Args&&... args)
    {
        Log(LogLevel::Info, fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    static void Warn(std::format_string<Args...> fmt, Args&&... args)
    {
        Log(LogLevel::Warning, fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    static void Error(std::format_string<Args...> fmt, Args&&... args)
    {
        Log(LogLevel::Error, fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    static void Debug(std::format_string<Args...> fmt, Args&&... args)
    {
        Log(LogLevel::Debug, fmt, std::forward<Args>(args)...);
    }

private:
    static std::string DoFormatMessage(LogLevel level, const std::string& msg, const std::source_location& loc)
    {
        auto now = std::chrono::system_clock::now();
        std::time_t itt = std::chrono::system_clock::to_time_t(now);

        std::tm tm{};
        localtime_s(&tm, &itt);

        char levelChar;
        switch (level)
        {
        case LogLevel::Info:
            levelChar = 'I';
            break;
        case LogLevel::Warning:
            levelChar = 'W';
            break;
        case LogLevel::Error:
            levelChar = 'E';
            break;
        case LogLevel::Debug:
            levelChar = 'D';
            break;
        }

        std::string timestamp = std::format("{:04}-{:02}-{:02} {:02}:{:02}:{:02}",
            tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
            tm.tm_hour, tm.tm_min, tm.tm_sec);

        if (level == LogLevel::Error)
        {
            std::string file = std::filesystem::path(loc.file_name()).filename().string();
            return std::format("{} [{}] {}:{} {} - {}", timestamp, levelChar, file, loc.line(), loc.function_name(), msg);
        }

        return std::format("{} [{}] {}", timestamp, levelChar, msg);
    }

    inline static std::mutex _mutex;
};
