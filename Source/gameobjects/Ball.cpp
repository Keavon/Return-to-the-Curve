#include "Ball.h"

#include "../Shape.h"
#include "../WindowManager.h"
#include "../engine/ColliderSphere.h"
#include "../engine/PhysicsObject.h"
#include "../engine/ParticleEmitter.h"
#include "../effects/ParticleSpark.h"
#include "Enemy.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/projection.hpp>
#include <glm/gtx/intersect.hpp>
#include <glm/glm.hpp>
#include <memory>
#include <cmath>
#include <iostream>
#include <unordered_set>

using namespace glm;
using namespace std;

Ball::Ball(vec3 position, quat orientation, shared_ptr<Shape> model, float radius) : PhysicsObject(position, orientation, model, make_shared<ColliderSphere>(radius)),
                                                                                     radius(radius)
{
    speed = 0;
    material = 0;

    moveForce = 200;
    acceleration = vec3(0);
    velocity = vec3(0);

    mass = 5;
    invMass = 1 / mass;
    elasticity = 0.5;

    friction = 0.25;

    jumpForce = 150;
}

void Ball::init(WindowManager *windowManager, shared_ptr<ParticleEmitter> sparkEmitter)
{
    this->windowManager = windowManager;
    this->sparkEmitter = sparkEmitter;
}

void Ball::update(float dt, glm::vec3 dolly, glm::vec3 strafe)
{
    for (auto collision : collider->pendingCollisions)
    {
        if (dynamic_cast<Enemy *>(collision.other) != NULL)
        {
            impulse -= collision.normal * 300.0f;
        }
    }

    PhysicsObject::update(dt);

    vec3 direction = vec3(0);
    if (glfwGetKey(windowManager->getHandle(), GLFW_KEY_W) == GLFW_PRESS)
    {
        direction += vec3(dolly.x, 0.0f, dolly.z);
    }
    if (glfwGetKey(windowManager->getHandle(), GLFW_KEY_S) == GLFW_PRESS)
    {
        direction -= vec3(dolly.x, 0.0f, dolly.z);
    }
    if (glfwGetKey(windowManager->getHandle(), GLFW_KEY_A) == GLFW_PRESS)
    {
        direction -= vec3(strafe.x, 0.0f, strafe.z);
    }
    if (glfwGetKey(windowManager->getHandle(), GLFW_KEY_D) == GLFW_PRESS)
    {
        direction += vec3(strafe.x, 0.0f, strafe.z);
    }
    if (glfwGetKey(windowManager->getHandle(), GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        if (normForce != vec3(0))
        {
            vec3 normForceDir = normalize(normForce);
            impulse += normForceDir * dot(vec3(0, jumpForce, 0), normForceDir);
        }
    }

    // calculate forces
    if (direction != vec3(0))
    {
        direction = normalize(direction);
        netForce += direction * moveForce;
    }

    if (velocity.x != 0 && velocity.y != 0)
    {
        vec3 axis = normalize(cross(vec3(0, 1, 0), velocity));
        quat q = rotate(quat(1, 0, 0, 0), length(vec2(velocity.x, velocity.z)) / radius * dt, axis);
        orientation = q * orientation;
    }


}

void Ball::onHardCollision(float impactVel, Collision &collision)
{
    if (impactVel > 5)
    {
        int numSparks = ((int) impactVel - 5) / 3;
        for (int i = 0; i < (int) impactVel; i++)
        {
            sparkEmitter->addParticle(make_shared<ParticleSpark>(collision.pos, impactVel, collision.normal));
        }
    }
}
