#include "Camera.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>
#include <algorithm>
#include <cstdlib>
using namespace std;
using namespace glm;

Camera::Camera(WindowManager *windowManager, glm::vec3 centerOfLevel) : windowManager(windowManager)
{
    levelCenterPoint = centerOfLevel;
}

Camera::~Camera()
{
}

void Camera::update(float dt, shared_ptr<Ball> ball)
{
    int windowWidth, windowHeight;
    glfwGetFramebufferSize(windowManager->getHandle(), &windowWidth, &windowHeight);

    // Handle movement input
    float speed_ = speed;
    vec3 velocity = vec3(0, 0, 0);
    strafe = normalize(cross(lookAtPoint - eye, upVec));
    dolly = normalize(lookAtPoint - eye);
    //Mouse calculations
    double xpos, ypos;
    glfwGetCursorPos(windowManager->getHandle(), &xpos, &ypos);
    double dx = xpos - prevXpos;
    double dy = -(ypos - prevYpos);
    prevXpos = xpos;
    prevYpos = ypos;
    double radPerPx = M_PI / windowHeight;
    yaw += dx * radPerPx;

    // Timer for camera path to start at the beginning of level
    /*
    double t;
    if (startTimer < 11){
        t = glfwGetTime();
        startTimer += std::min(t - prevTime, 0.1);
        //printf("StartTimer: %f",startTimer);
        if (9.0 < startTimer && startTimer < 10.0){
            previewLvl = true;
            startLvlPreview(levelCenterPoint);
        }
    } */

    if (flying)
    {
        dolly = normalize(lookAtPoint - eye);
        pitch = std::max(std::min(pitch + dy * radPerPx, radians(80.0)), -radians(80.0));
    }
    else if (previewLvl) {
        //printf("Dt: %f\n",dt);
        // Linear Zoom Out from Marble
        if (pathT < 0.02) {
            cameraPath->calcCircPos(theta, pathRadius, 'y', 20*cos(theta));
            circStartPos = cameraPath->getTargetPos();
            //printf("Circle Start Position: {%f,%f,%f}\n", circStartPos.x, circStartPos.y,circStartPos.z);
            pathVect = normalize(vec3{circStartPos.x - eye.x, circStartPos.y - eye.y, circStartPos.z - eye.z});
            pathSpeed = (float)glm::distance(circStartPos, eye) / (1.0 / 0.01);
            lookAtVec = normalize(vec3{ levelCenterPoint.x - lookAtPoint.x, 
                                        levelCenterPoint.y - lookAtPoint.y, 
                                        levelCenterPoint.z - lookAtPoint.z});
            lookAtPathSpeed = (float)glm::distance(levelCenterPoint, lookAtPoint) / ((1.0 - 0.4) / 0.01);
        }
        if (pathT < 1){
            if (pointReached) {
                pathVel = pathVect * pathSpeed;
                cameraPathTarget = eye + pathVel;
                //printf("Camera Path Target: {%f,%f,%f}\n",cameraPathTarget.x, cameraPathTarget.y,cameraPathTarget.z);
                if (pathT > 0.4){
                    lookAtPathVel = lookAtVec * lookAtPathSpeed;
                    lookAtTarget = lookAtPoint + lookAtPathVel;
                }
                pathT += 0.01;
                pointReached = false;
            }
            pathVel = pathVect * (float)0.5;
            eye += pathVel;
            if (pathT > 0.4){
                lookAtPathVel = lookAtVec * (float)0.8;
                lookAtPoint += lookAtPathVel;
            }
            if (glm::distance(eye, cameraPathTarget) < 1){
                pointReached = true;
            }
            //printf("PathT: %f\n", pathT);
        }
        else {
            lookAtPoint = levelCenterPoint;
            pathSpeed = 0.5;
            if (theta < 6.28){
                if (pointReached) {
                    theta += 0.02;
                    cameraPath->calcCircPos(theta, pathRadius, 'y', (20*cos(theta*2) + 10) );
                    circNextPos = cameraPath->getTargetPos();
                    pathVect = normalize( vec3{ circNextPos.x - eye.x, 
                                                circNextPos.y - eye.y, 
                                                circNextPos.z - eye.z});
                    //printf("Theta: %f\nCircNextPos: (%f, %f, %f)\n", theta, circNextPos.x, circNextPos.y, circNextPos.z);
                    pointReached = false;
                }
                pathVel = pathVect * pathSpeed;
                eye += pathVel;
                if (glm::distance(eye, circNextPos) < 0.6){
                    pointReached = true;
                }
            }
            else {
                previewLvl = false;
            }
        }
    }
    else
    {
        pitch = std::max(std::min(pitch + dy * radPerPx, radians(80.0)), radians(10.0));
        lookAtPoint = ball->position;
        eye.x = lookAtPoint.x + distToBall * cos(pitch) * sin(yaw);
        eye.y = lookAtPoint.y + distToBall * sin(pitch);
        eye.z = lookAtPoint.z + distToBall * cos(pitch) * cos(M_PI - yaw); 
    }
    if (!previewLvl) {
        
        if (glfwGetKey(windowManager->getHandle(), GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS ||
            glfwGetKey(windowManager->getHandle(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        {
            speed_ *= sprintFactor;
        }
        if (glfwGetKey(windowManager->getHandle(), GLFW_KEY_I) == GLFW_PRESS)
        {
            velocity += dolly;
        }
        if (glfwGetKey(windowManager->getHandle(), GLFW_KEY_K) == GLFW_PRESS)
        {
            velocity -= dolly;
        }
        if (glfwGetKey(windowManager->getHandle(), GLFW_KEY_L) == GLFW_PRESS)
        {
            velocity += strafe;
        }
        if (glfwGetKey(windowManager->getHandle(), GLFW_KEY_J) == GLFW_PRESS)
        {
            velocity -= strafe;
        }
        if (glfwGetKey(windowManager->getHandle(), GLFW_KEY_M) == GLFW_PRESS)
        {
            velocity += vec3(0, 1, 0);
        }
        if (glfwGetKey(windowManager->getHandle(), GLFW_KEY_N) == GLFW_PRESS)
        {
            velocity -= vec3(0, 1, 0);
        }
        if (length(velocity) != 0 && length(velocity) != NAN)
        {
            eye += normalize(velocity) * speed_ * (float) dt;
        }
    }
    // Mouse
    
    if (flying){
        lookAtPoint.x = eye.x + cos(pitch) * sin(yaw);
        lookAtPoint.y = eye.y + sin(pitch);
        lookAtPoint.z = eye.z + cos(pitch) * cos(M_PI - yaw);
    } 
}
glm::vec3 Camera::getDolly() {
    if (flying){
        return vec3(0,0,-1);
    }
    return this->dolly;
}

glm::vec3 Camera::getStrafe() {
    if (flying){
        return vec3(1,0,0);
    }
    return this->strafe;
}

void Camera::init()
{
    glfwGetCursorPos(windowManager->getHandle(), &prevXpos, &prevYpos);
    eye = vec3(0, this->height, 0);
    lookAtPoint = eye + vec3(1, 0, 0);
    upVec = vec3(0, 1, 0);
    pitch = 0;
    yaw = 10;
    cameraPath = new Pathing();

    prevTime = glfwGetTime();
    startTimer = 0;
    
}

void Camera::startLvlPreview(glm::vec3 lvlCenterPt) {
    pathT = 0.01;
    pointReached = true;
    cameraStartPos = eye;
    levelCenterPoint = lvlCenterPt;
    theta = 0;
}