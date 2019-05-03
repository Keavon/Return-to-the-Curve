#include "PhysicsObject.h"

#include "ColliderSphere.h"
#include "Collider.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/projection.hpp>
#include <glm/glm.hpp>
#include <memory>
#include <iostream>
#include <cmath>

using namespace std;
using namespace glm;

bool inRange(float n, float low, float high)
{
    return low <= n && n <= high;
}

PhysicsObject::PhysicsObject(vec3 position, quat orientation,
    shared_ptr<Shape> model, shared_ptr<Collider> collider) :
    GameObject(position, orientation, model), collider(collider),
    netForce(0), impulse(0)
{
}

void PhysicsObject::update(float dt)
{
    netForce.y += GRAVITY * mass;

    // Resolve collision
    if (collider->pendingCollision.hit)
    {
        position = collider->pendingCollision.position;
        velocity -= (1.0f + elasticity) * proj(velocity, collider->pendingCollision.normal);
        position += velocity * collider->pendingCollision.t;
        netForce -= collider->pendingCollision.normal * netForce;
    }
    collider->pendingCollision = {};

    velocity += impulse;
    impulse = vec3(0);

    // apply force
    acceleration = netForce / mass;
    velocity += acceleration * dt;
    position += velocity * dt;

    netForce = vec3(0);
}

void PhysicsObject::checkCollision(PhysicsObject *other)
{
    collider->checkCollision(this, other, other->collider.get());
}