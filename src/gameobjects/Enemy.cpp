#include "Enemy.h"

#include "../Shape.h"
#include "../WindowManager.h"
#include "../engine/ColliderSphere.h"

#include <glm/glm.hpp>
#include <memory>
#include <cmath>

using namespace glm;
using namespace std;

Enemy::Enemy(float radius, vec3 position) :
    radius(radius)
{
    this->position = position;
    collider = ColliderSphere(radius);
    speed = 0;
    material = 0;
    orientation = quat(1, 0, 0, 0);

    moveSpeed = 5;
    acceleration = vec3(0, -20, 0);
    direction = vec3(0);
}

void Enemy::init(shared_ptr<Shape> model, WindowManager *windowManager)
{
    this->model = model;
    this->windowManager = windowManager;
}

void Enemy::update(float dt, glm::vec3 dolly, glm::vec3 strafe)
{
    direction = vec3(0);
    if (glfwGetKey(windowManager->getHandle(), GLFW_KEY_I) == GLFW_PRESS)
    {
        direction += vec3(dolly.x, 0, dolly.z);
    }
    if (glfwGetKey(windowManager->getHandle(), GLFW_KEY_K) == GLFW_PRESS)
    {
        direction -= vec3(dolly.x, 0, dolly.z);
    }
    if (glfwGetKey(windowManager->getHandle(), GLFW_KEY_J) == GLFW_PRESS)
    {
        direction -= vec3(strafe.x, 0, strafe.z);
    }
    if (glfwGetKey(windowManager->getHandle(), GLFW_KEY_L) == GLFW_PRESS)
    {
        direction += vec3(strafe.x, 0, strafe.z);
    }
    if (glfwGetKey(windowManager->getHandle(), GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        velocity.y = 10;
    }

    velocity.x = velocity.z = 0;
    if (length(direction) > 0)
    {
        direction = normalize(direction);
        vec3 axis = normalize(cross(vec3(0, 1, 0), direction));
        quat q = rotate(quat(1, 0, 0, 0), moveSpeed / radius * dt, axis);
        orientation = q * orientation;
        velocity += direction * moveSpeed;
    }

    velocity += acceleration * dt;
    position += velocity * dt;

    if (position.y < radius)
    {
        velocity.y = 0;
        position.y = radius;
    }
}