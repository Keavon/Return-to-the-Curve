#include "Camera.h"

#include <cmath>
#include <algorithm>
#include <cstdlib>
using namespace std;
using namespace glm;

Camera::Camera(WindowManager *windowManager) : windowManager(windowManager)
{
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

    if (flying)
    {
        dolly = normalize(lookAtPoint - eye);
        pitch = std::max(std::min(pitch + dy * radPerPx, radians(80.0)), -radians(80.0));
    }
    else
    {
        pitch = std::max(std::min(pitch + dy * radPerPx, radians(80.0)), radians(10.0));
        lookAtPoint = ball->position;
        eye.x = lookAtPoint.x + distToBall * cos(pitch) * sin(yaw);
        eye.y = lookAtPoint.y + distToBall * sin(pitch);
        eye.z = lookAtPoint.z + distToBall * cos(pitch) * cos(M_PI - yaw); 
    }

    if (glfwGetKey(windowManager->getHandle(), GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS ||
        glfwGetKey(windowManager->getHandle(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
        speed_ *= sprintFactor;
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
    if (glfwGetKey(windowManager->getHandle(), GLFW_KEY_E) == GLFW_PRESS)
    {
        velocity += vec3(0, 1, 0);
    }
    if (glfwGetKey(windowManager->getHandle(), GLFW_KEY_Q) == GLFW_PRESS)
    {
        velocity -= vec3(0, 1, 0);
    }
    if (length(velocity) != 0 && length(velocity) != NAN)
    {
        eye += normalize(velocity) * speed_ * (float) dt;
        eye.y = std::max(0.025f, eye.y);
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
}