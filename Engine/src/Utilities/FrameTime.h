#pragma once 

#include <chrono>

class FrameTime
{
public:
    FrameTime();
    ~FrameTime() = default;

    float GetTime() const { return _totalTime; }
    float GetDeltaTime() const { return _deltaTime; }
    int GetFPS() const { return _fps; }

    void Tick();

private:
    float _totalTime = 0.0f;
    float _deltaTime = 0.0f;

    int _fps = 0;
    int _frameCount = 0;

    float _elapsedTime = 0.0f;

    std::chrono::steady_clock::time_point _startTime;
    std::chrono::steady_clock::time_point _lastFrameTime;
};