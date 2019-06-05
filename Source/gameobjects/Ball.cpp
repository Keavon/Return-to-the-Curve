#include "Ball.h"

#include "../Shape.h"
#include "../WindowManager.h"
#include "../engine/ColliderSphere.h"
#include "../engine/PhysicsObject.h"
#include "../engine/ParticleEmitter.h"
#include "../effects/ParticleSpark.h"
#include "../effects/Sound.h"
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

Ball::Ball(vec3 position, quat orientation, shared_ptr<Shape> model, float radius) : PhysicsObject(position, orientation, model, make_shared<ColliderSphere>(radius)), radius(radius)
{
    speed = 0;
    material = 0;

    moveForce = 200;
    acceleration = vec3(0);
    velocity = vec3(0);

    jumpForce = 100;

    frozen = false;

    JUMP_TIME = 0.0f;
    WANTS_JUMP = 0;

    LAND_TIME = 0.0f;
    CAN_JUMP = 0;
    LAST_NORMAL_FORCE = vec3(1.0f);

    JUMPED_AT_TIME = 0.0f;
    JUST_JUMPED = 0;
    this->startPosition = position;
    this->moveForce = 200;
    this->jumpForce = 150;
    this->frozen = false;

    setMass(5);
    setElasticity(0.5);
    setFriction(0.25);
}

void Ball::init(WindowManager *windowManager, shared_ptr<ParticleEmitter> sparkEmitter)
{
    this->windowManager = windowManager;
    this->sparkEmitter = sparkEmitter;
}

void Ball::update(float dt, glm::vec3 dolly, glm::vec3 strafe)
{

    if (frozen)
        return;

    for (auto collision : collider->pendingCollisions)
    {
        if (dynamic_cast<Enemy *>(collision.other) != NULL)
        {
            applyImpulse(-collision.normal * 300.0f);
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

    //===============================================================================
    //Determine Jump
    //===============================================================================
    float currentTime = glfwGetTime();
    vec3 normForceDir = vec3(0.0f);
    if (normForce != vec3(0))
    {
        // Just landed, or continuing to contact ground, THERFORE a jump is allowed.
        LAND_TIME = currentTime;
        CAN_JUMP = 1;
        LAST_NORMAL_FORCE = normForce;
    }
    if (glfwGetKey(windowManager->getHandle(), GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        // User pressed space, THERFORE they want to jump.
        JUMP_TIME = currentTime;
        WANTS_JUMP = 1;
    }

    // Any of the three timers expire?
    if (WANTS_JUMP && (currentTime - JUMP_TIME) >= 0.125)
    {
        WANTS_JUMP = 0;
    }
    if (CAN_JUMP && (currentTime - LAND_TIME) >= 0.125)
    {
        CAN_JUMP = 0;
    }
    if (JUST_JUMPED && (currentTime - JUMPED_AT_TIME) >= 0.25)
    {
        JUST_JUMPED = 0;
    }

    // Can we? Do we want to? Have we not recently?
    if (CAN_JUMP && WANTS_JUMP && !JUST_JUMPED)
    {
        normForceDir = normalize(LAST_NORMAL_FORCE);
        applyImpulse(normForceDir * dot(vec3(0, jumpForce, 0), normForceDir));
        CAN_JUMP = 0;
        WANTS_JUMP = 0;
        JUST_JUMPED = 1;
        JUMPED_AT_TIME = currentTime;
    }
    //===============================================================================

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
    if (impactVel > 4.0)
    {
        soundEngine->impact(impactVel);

        int numSparks = ((int)impactVel - 5) / 3;
        for (int i = 0; i < (int)impactVel; i++)
        {
            sparkEmitter->addParticle(make_shared<ParticleSpark>(collision.pos, impactVel, collision.normal));
        }
    }
}

void Ball::addSkin(std::shared_ptr<Material> newSkin)
{
    marbleSkins.push_back(newSkin);
}

void Ball::setSkin(int skinIndex)
{
    currentSkin = skinIndex;
}

void Ball::nextSkin()
{
    currentSkin++;
    currentSkin %= marbleSkins.size();
}

shared_ptr<Material> Ball::getSkinMaterial()
{
    return marbleSkins[currentSkin];
}