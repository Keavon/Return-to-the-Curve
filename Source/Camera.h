#pragma once

#include "Pathing.h"
#include "WindowManager.h"
#include "gameobjects/Ball.h"
#include <glm/glm.hpp>

class Camera {
public:
    enum CameraMode { marble, edit, flythrough };

    Camera(WindowManager *windowManager, glm::vec3 centerOfLevel);
    ~Camera();
    void update(float dt, std::shared_ptr<Ball> ball);
    void init();
    void startLvlPreview(glm::vec3 lvlCenterPt);
    void marbleModeUpdate(shared_ptr<Ball> ball);
    void editModeUpdate(float dt);
    void flythroughModeUpdate();
    void saveMarbleView();
    void restoreMarbleView();
    glm::vec3 getDolly();
    glm::vec3 getStrafe();

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

    glm::vec3 eye;
    glm::vec3 lookAtPoint;
    glm::vec3 upVec;
    glm::vec3 dolly;
    glm::vec3 strafe;

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

    glm::vec3 cameraPathTarget;
    glm::vec3 cameraStartPos;
    glm::vec3 circStartPos;
    glm::vec3 circNextPos;
    glm::vec3 levelCenterPoint;
    glm::vec3 lookAtPathVel;
    glm::vec3 lookAtTarget;
    glm::vec3 lookAtVec;
    glm::vec3 pathVect;
    glm::vec3 pathVel;
};