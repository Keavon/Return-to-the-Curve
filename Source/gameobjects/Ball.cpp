#include "Ball.h"

#include "../Camera.h"
#include "Enemy.h"

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
    
    this->moveForce = 200;
    this->jumpForce = 150;
    this->frozen = false;
    this->initialized = false;

    setMass(5);
    setElasticity(0.5);
    setFriction(0.25);
}

void Ball::init(vec3 startPosition, WindowManager *windowManager, shared_ptr<ParticleEmitter> sparkEmitter, shared_ptr<Camera> camera)
{
    this->startPosition = position;
    this->windowManager = windowManager;
    this->sparkEmitter = sparkEmitter;
    this->camera = camera;
    this->initialized = true;
}

void Ball::collectedPowerUp(string type)
{
    powerups.push(type);
}

void Ball::update()
{
    if (!initialized) return;

    if (frozen) return;

    for (auto collision : collider->pendingCollisions)
    {
        if (dynamic_cast<Enemy *>(collision.other) != NULL)
        {
            applyImpulse(-collision.normal * 300.0f);
        }
    }

    PhysicsObject::update();

    vec3 direction = vec3(0);
    if (glfwGetKey(windowManager->getHandle(), GLFW_KEY_W) == GLFW_PRESS)
    {
        direction += vec3(camera->dolly.x, 0.0f, camera->dolly.z);
    }
    if (glfwGetKey(windowManager->getHandle(), GLFW_KEY_S) == GLFW_PRESS)
    {
        direction -= vec3(camera->dolly.x, 0.0f, camera->dolly.z);
    }
    if (glfwGetKey(windowManager->getHandle(), GLFW_KEY_A) == GLFW_PRESS)
    {
        direction -= vec3(camera->strafe.x, 0.0f, camera->strafe.z);
    }
    if (glfwGetKey(windowManager->getHandle(), GLFW_KEY_D) == GLFW_PRESS)
    {
        direction += vec3(camera->strafe.x, 0.0f, camera->strafe.z);
    }

    //===============================================================================
    //Determine Jump
    //===============================================================================
    float currentTime = (float)glfwGetTime();
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
    if (glfwGetKey(windowManager->getHandle(), GLFW_KEY_E) == GLFW_PRESS)
    {
        activatePowerUp();
    }
    if (glfwGetMouseButton(windowManager->getHandle(), GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
        activatePowerUp();
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
        soundEngine->jump();
        normForceDir = normalize(LAST_NORMAL_FORCE);
        applyImpulse(normForceDir * dot(vec3(0, jumpForce, 0), normForceDir));
        CAN_JUMP = 0;
        WANTS_JUMP = 0;
        JUST_JUMPED = 1;
        JUMPED_AT_TIME = currentTime;

        if (jumpForce > 150)
        {
            soundEngine->superBounce();
            jumpForce = 150;
        }
    }
    
    if (moveForce > 200)
    {
        if (currentTime - POWER_UP_START_TIME > 3){
            moveForce = 200;
        }
    }
    // calculate forces
    if (direction != vec3(0))
    {
        direction = normalize(direction);
        netForce += direction * moveForce;
    }

    if (velocity.x != 0 || velocity.z != 0)
    {
        vec3 axis = normalize(cross(vec3(0, 1, 0), velocity));
        quat q = rotate(quat(1, 0, 0, 0), length(vec2(velocity.x, velocity.z)) / radius * Time.physicsDeltaTime, axis);
        if (!isnan(q.w))
        {
            orientation = q * orientation;
        }
    }
}

void Ball::onHardCollision(float impactVel, Collision &collision)
{
    if (!initialized) return;
    
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

void Ball::addSkin(shared_ptr<Material> newSkin)
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

void Ball::activatePowerUp() 
{
    if ((float)glfwGetTime() - lastPowerupActivationTime < 1) return;

    lastPowerupActivationTime = (float)glfwGetTime();

    if (powerups.size() > 0 && powerups.front() == "Super Jump") {
        jumpForce = 500;
        powerups.pop();
    }
    else if (powerups.size() > 0 && powerups.front() == "Lightning Speed")
    {
        POWER_UP_START_TIME = (float)glfwGetTime();
        moveForce = 400;
        powerups.pop();
    }
}
