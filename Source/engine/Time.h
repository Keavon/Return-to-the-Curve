#pragma once

struct TimeData {
    float timeSinceStart;
    float deltaTime;
    float physicsDeltaTime;
};

extern TimeData Time;