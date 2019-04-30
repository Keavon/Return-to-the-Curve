#include "Ball.h"

#include "../Shape.h"
#include "../WindowManager.h"
#include "../engine/ColliderSphere.h"
#include "../engine/PhysicsObject.h"

#include <glm/glm.hpp>
#include <memory>
#include <cmath>
#include <iostream>

using namespace glm;
using namespace std;

Ball::Ball(vec3 position, quat orientation, shared_ptr<Shape> model, float radius) :
    PhysicsObject(position, orientation, model, make_shared<ColliderSphere>(this, radius)),
    radius(radius)
{
    speed = 0;
    material = 0;

    moveForce = 100;
    acceleration = vec3(0);
    velocity = vec3(0);

    mass = 10;
    elasticity = 0.5;
}

void Ball::init(WindowManager *windowManager)
{
    this->windowManager = windowManager;
}

void Ball::update(float dt)
{
    vec3 direction = vec3(0);
    if (glfwGetKey(windowManager->getHandle(), GLFW_KEY_I) == GLFW_PRESS)
    {
        direction += vec3(0, 0, -1);
    }
    if (glfwGetKey(windowManager->getHandle(), GLFW_KEY_K) == GLFW_PRESS)
    {
        direction += vec3(0, 0, 1);
    }
    if (glfwGetKey(windowManager->getHandle(), GLFW_KEY_J) == GLFW_PRESS)
    {
        direction += vec3(-1, 0, 0);
    }
    if (glfwGetKey(windowManager->getHandle(), GLFW_KEY_L) == GLFW_PRESS)
    {
        direction += vec3(1, 0, 0);
    }
    if (glfwGetKey(windowManager->getHandle(), GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        // velocity.y = 10;
        netForce.y += 1000;
    }

    // calculate forces
    if (length(direction) > 0)
    {
        direction = normalize(direction);
        netForce += direction * moveForce;
    }

    if (length(vec2(velocity.x, velocity.z)) > 0)
    {
        vec3 axis = normalize(cross(vec3(0, 1, 0), velocity));
        quat q = rotate(quat(1, 0, 0, 0), length(vec2(velocity.x, velocity.z)) / radius * dt, axis);
        orientation = q * orientation;
    }

    PhysicsObject::update(dt);


    if (position.y < radius)
    {
        velocity.y = 0;
        position.y = radius;
    }

}