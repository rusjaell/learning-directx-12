#include "FrameTime.h"

#include <cmath>
#include <iostream>
#include <string>

#include "Logger.h"

FrameTime::FrameTime()
    : _startTime(std::chrono::steady_clock::now()), _lastFrameTime(_startTime)
{
}

void FrameTime::Tick()
{
    _frameCount++;

    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();

    _deltaTime = std::chrono::duration<float>(now - _lastFrameTime).count();
    _lastFrameTime = now;

    _totalTime += _deltaTime;
    _elapsedTime += _deltaTime;

    if (_elapsedTime >= 1.0f)
    {
        _fps = static_cast<int>(std::round(_frameCount / _elapsedTime));

        float fps = _fps;
        float deltaTime = _deltaTime * 1000.0f;
        float totalTime = _totalTime;
        Logger::Debug("FPS: {:.2f} | DT: {:.2f} ms | T: {:.2f}", fps, deltaTime, totalTime);

        _frameCount = 0;
        _elapsedTime = 0.0f;
    }
}