#include "Camera.h"

Camera::Camera(WindowManager *windowManager, vec3 centerOfLevel) : windowManager(windowManager)
{
    levelCenterPoint = centerOfLevel;
}

Camera::~Camera() {}

void Camera::update(shared_ptr<Ball> ball)
{
    glfwGetFramebufferSize(windowManager->getHandle(), &windowWidth, &windowHeight);

    // Handle movement input
    strafe = normalize(cross(lookAtPoint - eye, upVec));
    dolly = normalize(lookAtPoint - eye);

    // Mouse calculations
    glfwGetCursorPos(windowManager->getHandle(), &xpos, &ypos);

    dx = xpos - prevXpos;
    dy = -(ypos - prevYpos);
    prevXpos = xpos;
    prevYpos = ypos;

    radPerPx = M_PI / windowHeight;

    if (cameraMode == marble)
        marbleModeUpdate(ball);
    else if (cameraMode == edit)
        editModeUpdate();
    //else if (cameraMode == flythrough) flythroughModeUpdate();
}

void Camera::marbleModeUpdate(shared_ptr<Ball> ball)
{
    glfwSetInputMode(windowManager->getHandle(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (freeViewing)
    {
        pitch -= dy * radPerPx;
        pitch = std::max(std::min(pitch, radians(89.9)), radians(0.0));
    }

    lookAtPoint = ball->position;

    yaw += dx * radPerPx;
    eye.x = lookAtPoint.x + distToBall * (float)cos(pitch) * (float)sin(yaw);
    eye.y = lookAtPoint.y + distToBall * (float)sin(pitch);
    eye.z = lookAtPoint.z + distToBall * (float)cos(pitch) * (float)cos(M_PI - yaw);
}

void Camera::editModeUpdate()
{
    float speed = moveSpeed;
    vec3 velocity = vec3(0, 0, 0);
    glfwSetInputMode(windowManager->getHandle(), GLFW_CURSOR, !freeViewing ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);

    if (glfwGetKey(windowManager->getHandle(), GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS ||
        glfwGetKey(windowManager->getHandle(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
        speed *= sprintFactor;
    }
    if (glfwGetKey(windowManager->getHandle(), GLFW_KEY_W) == GLFW_PRESS)
    {
        velocity += dolly;
    }
    if (glfwGetKey(windowManager->getHandle(), GLFW_KEY_S) == GLFW_PRESS)
    {
        velocity -= dolly;
    }
    if (glfwGetKey(windowManager->getHandle(), GLFW_KEY_D) == GLFW_PRESS)
    {
        velocity += strafe;
    }
    if (glfwGetKey(windowManager->getHandle(), GLFW_KEY_A) == GLFW_PRESS)
    {
        velocity -= strafe;
    }
    if (glfwGetKey(windowManager->getHandle(), GLFW_KEY_Q) == GLFW_PRESS)
    {
        velocity += cross(dolly, strafe);
    }
    if (glfwGetKey(windowManager->getHandle(), GLFW_KEY_E) == GLFW_PRESS)
    {
        velocity -= cross(dolly, strafe);
    }
    if (length(velocity) != 0 && length(velocity) != NAN)
    {
        eye += normalize(velocity) * speed * Time.deltaTime;
    }

    if (freeViewing)
    {
        pitch -= dy * radPerPx;
        pitch = std::max(std::min(pitch, radians(89.9)), -radians(89.9));

        yaw += dx * radPerPx;

        lookAtPoint.x = eye.x + (float)cos(-pitch) * (float)sin(yaw + M_PI);
        lookAtPoint.y = eye.y + (float)sin(-pitch);
        lookAtPoint.z = eye.z + (float)cos(-pitch) * (float)cos(yaw);
    }
}

//void Camera::flythroughModeUpdate() {
//    // Linear Zoom Out from Marble
//    if (pathT < 0.02) {
//        cameraPath->calcCircPos(theta, pathRadius, 'y', 20 * cos(theta));
//        circStartPos = cameraPath->getTargetPos();
//        //printf("Circle Start Position: {%f,%f,%f}\n", circStartPos.x, circStartPos.y,circStartPos.z);
//        pathVect = normalize(vec3{ circStartPos.x - eye.x, circStartPos.y - eye.y, circStartPos.z - eye.z });
//        pathSpeed = (float)distance(circStartPos, eye) / (1.0 / 0.01);
//        lookAtVec = normalize(vec3{ levelCenterPoint.x - lookAtPoint.x,
//                                    levelCenterPoint.y - lookAtPoint.y,
//                                    levelCenterPoint.z - lookAtPoint.z });
//        lookAtPathSpeed = (float)distance(levelCenterPoint, lookAtPoint) / ((1.0 - 0.4) / 0.01);
//    }
//    if (pathT < 1) {
//        if (pointReached) {
//            pathVel = pathVect * pathSpeed;
//            cameraPathTarget = eye + pathVel;
//            //printf("Camera Path Target: {%f,%f,%f}\n",cameraPathTarget.x, cameraPathTarget.y,cameraPathTarget.z);
//            if (pathT > 0.4) {
//                lookAtPathVel = lookAtVec * lookAtPathSpeed;
//                lookAtTarget = lookAtPoint + lookAtPathVel;
//            }
//            pathT += 0.01;
//            pointReached = false;
//        }
//        pathVel = pathVect * (float)0.5;
//        eye += pathVel;
//        if (pathT > 0.4) {
//            lookAtPathVel = lookAtVec * (float)0.8;
//            lookAtPoint += lookAtPathVel;
//        }
//        if (distance(eye, cameraPathTarget) < 1) {
//            pointReached = true;
//        }
//        //printf("PathT: %f\n", pathT);
//    }
//    else {
//        lookAtPoint = levelCenterPoint;
//        pathSpeed = 0.5;
//        if (theta < 6.28) {
//            if (pointReached) {
//                theta += 0.02;
//                cameraPath->calcCircPos(theta, pathRadius, 'y', (20 * cos(theta * 2) + 10));
//                circNextPos = cameraPath->getTargetPos();
//                pathVect = normalize(vec3{ circNextPos.x - eye.x,
//                                            circNextPos.y - eye.y,
//                                            circNextPos.z - eye.z });
//                //printf("Theta: %f\nCircNextPos: (%f, %f, %f)\n", theta, circNextPos.x, circNextPos.y, circNextPos.z);
//                pointReached = false;
//            }
//            pathVel = pathVect * pathSpeed;
//            eye += pathVel;
//            if (distance(eye, circNextPos) < 0.6) {
//                pointReached = true;
//            }
//        }
//        else {
//            previewLvl = false;
//        }
//    }
//}

void Camera::saveMarbleView()
{
    savedPitch = pitch;
    savedYaw = yaw;
    savedDistToBall = distToBall;
}

void Camera::restoreMarbleView()
{
    pitch = savedPitch;
    yaw = savedYaw;
    distToBall = savedDistToBall;
}

void Camera::init()
{
    glfwGetCursorPos(windowManager->getHandle(), &prevXpos, &prevYpos);
    eye = vec3(0, this->height, 0);
    lookAtPoint = eye + vec3(1, 0, 0);
    upVec = vec3(0, 1, 0);
    pitch = radians(20.0);
    yaw = 10;
    distToBall = 15;
    cameraPath = new Pathing();

    prevTime = glfwGetTime();
    startTimer = 0;
}
// TODO:: fix to start with a given bezierCurve possibly
void Camera::startLvlPreview(vec3 lvlCenterPt)
{
    pathT = 0.01f;
    pointReached = true;
    cameraStartPos = eye;
    levelCenterPoint = lvlCenterPt;
    theta = 0;
}