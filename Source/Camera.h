#pragma once

#define _USE_MATH_DEFINES

#include <cmath>
#include <algorithm>
#include <cstdlib>
#include <glm/glm.hpp>
#include <math.h>

#include "Pathing.h"
#include "WindowManager.h"
#include "gameobjects/Ball.h"
#include "engine/Time.h"

using namespace std;
using namespace glm;

class Camera {
public:
    enum CameraMode { marble, edit, flythrough };

    Camera(WindowManager *windowManager, vec3 centerOfLevel);
    ~Camera();
    void update(shared_ptr<Ball> ball);
    void init();
    void startLvlPreview(vec3 lvlCenterPt);
    void marbleModeUpdate(shared_ptr<Ball> ball);
    void editModeUpdate();
    void flythroughModeUpdate();
    void saveMarbleView();
    void restoreMarbleView();

    float moveSpeed = 10;
    float sprintFactor = 3;
    float radius = 1.5;
    float height = 5;
    bool freeViewing = false;
    CameraMode cameraMode = marble;
    Pathing *cameraPath;
    WindowManager *windowManager;

    // Camera data
    double prevXpos;
    double prevYpos;

    double pitch;
    double yaw;
    float distToBall;
    double savedPitch;
    double savedYaw;
    float savedDistToBall;

    vec3 eye;
    vec3 lookAtPoint;
    vec3 upVec;
    vec3 dolly;
    vec3 strafe;

    //Camera Path Data
    bool pointReached;
    float lookAtPathSpeed;
    float pathRadius = 100;
    float pathSpeed;
    float pathT;
    double prevTime;
    float startTimer;
    float theta;

    // Updated-per-frame data
    int windowWidth;
    int windowHeight;
    double xpos;
    double ypos;
    double dx;
    double dy;
    double radPerPx;

    vec3 cameraPathTarget;
    vec3 cameraStartPos;
    vec3 circStartPos;
    vec3 circNextPos;
    vec3 levelCenterPoint;
    vec3 lookAtPathVel;
    vec3 lookAtTarget;
    vec3 lookAtVec;
    vec3 pathVect;
    vec3 pathVel;
};