#include "Ball.h"

#include "../Shape.h"
#include "../WindowManager.h"
#include "../engine/ColliderSphere.h"
#include "../engine/PhysicsObject.h"
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

    mass = 10;
    invMass = 1 / mass;
    elasticity = 0.5;

    friction = 0.25;

    jumpForce = 200;

    SoundEngine = irrklang::createIrrKlangDevice();
}

void Ball::init(WindowManager *windowManager)
{
    this->windowManager = windowManager;
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

    // filter collisions so that ball doesn't bump over edges
    vector<Collision *> faceCollisions;
    vector<Collision *> notFaceCollisions;
    for (int i = 0; i < collider->pendingCollisions.size(); i++)
    {
        switch (collider->pendingCollisions[i].geom)
        {
        case FACE:
            faceCollisions.push_back(&collider->pendingCollisions[i]);
            break;
        case EDGE:
        case VERT:
            notFaceCollisions.push_back(&collider->pendingCollisions[i]);
            break;
        }
    }
    unordered_set<Collision *> collisionsToRemove;
    for (int i = 0; i < faceCollisions.size(); i++)
    {
        for (int j = 0; j < notFaceCollisions.size(); j++)
        {
            if (faceCollisions[i]->other != notFaceCollisions[j]->other)
            {
                float d;
                intersectRayPlane(notFaceCollisions[j]->pos, -faceCollisions[i]->normal,
                                  faceCollisions[i]->v[0], faceCollisions[i]->normal, d);
                if (d < 0.1)
                {
                    collisionsToRemove.insert(notFaceCollisions[j]);
                }
            }
        }
    }
    for (int i = collider->pendingCollisions.size() - 1; i >= 0 && !collisionsToRemove.empty(); i--)
    {
        Collision *col = &collider->pendingCollisions[i];
        if (collisionsToRemove.find(col) != collisionsToRemove.end())
        {
            collisionsToRemove.erase(col);
            collider->pendingCollisions.erase(collider->pendingCollisions.begin() + i);
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

            SoundEngine->play2D("../Resources/sounds/jump.wav", GL_FALSE);
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