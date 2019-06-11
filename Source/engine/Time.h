#pragma once

struct TimeData {
    float timeSinceStart;
    float deltaTime;
    float physicsDeltaTime;
    float musicDeltaTime;
};

extern TimeData Time;