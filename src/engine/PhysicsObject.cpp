#include "PhysicsObject.h"

#include "ColliderSphere.h"
#include "Collider.h"

#define NOMINMAX
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/projection.hpp>
#include <glm/glm.hpp>
#include <memory>
#include <iostream>
#include <cmath>
#include <algorithm>

using namespace std;
using namespace glm;

bool inRange(float n, float low, float high)
{
    return low <= n && n <= high;
}

PhysicsObject::PhysicsObject(vec3 position, quat orientation,
    shared_ptr<Shape> model, shared_ptr<Collider> collider) :
    GameObject(position, orientation, model), collider(collider),
    netForce(0), impulse(0), acceleration(0), velocity(0), mass(0), invMass(0),
    normForce(0), friction(0), elasticity(0), speed(0)
{
}
void PhysicsObject::update(float dt)
{
    normForce = vec3(0);
    netForce.y += GRAVITY * mass;

    for (Collision collision : collider->pendingCollisions)
    {
        // resolve collision
        PhysicsObject *other = collision.other;
        vec3 relVel = other->velocity - velocity;
        float velAlongNormal = dot(relVel, collision.normal);
        if (velAlongNormal < 0)
        {
            float e = (std::min)(other->elasticity, elasticity);
            float j = (-(1 + e) * velAlongNormal) / (invMass + other->invMass);
            vec3 colImpulse = j * collision.normal;
            velocity -= invMass * colImpulse;

            // normal force
            vec3 localNormForce = collision.normal * dot(netForce, -collision.normal);
            normForce += localNormForce;

            // friction
            vec3 frictionDir = (relVel - proj(relVel, collision.normal));
            if (frictionDir != vec3(0))
            {
                frictionDir = normalize(frictionDir);
                netForce += length(localNormForce) * friction * frictionDir;
            }

            // correct position to prevent sinking/jitter
            if (other->invMass == 0)
            {
                float percent = 0.2;
                float slop = 0.01;
                vec3 correction = (std::max)(collision.penetration - slop, 0.0f) / (invMass + other->invMass) * percent * -collision.normal;
                position += invMass * correction;
            }
        }
    }
    collider->pendingCollisions.clear();

    netForce += normForce;

    velocity += impulse * invMass;

    // drag
    if (velocity != vec3(0))
    {
        netForce += dot(velocity, velocity) * -normalize(velocity) * DRAG_COEFFICIENT;
    }

    // apply force
    acceleration = netForce * invMass;
    velocity += acceleration * dt;
    position += velocity * dt;

    impulse = vec3(0);
    netForce = vec3(0);
}

void PhysicsObject::checkCollision(PhysicsObject *other)
{
    if (other->collider != NULL)
    {
        collider->checkCollision(this, other, other->collider.get());
    }
}

float PhysicsObject::getRadius()
{
    if (collider == NULL)
    {
        return 0;
    }
    else
    {
        return (std::max)({scale.x, scale.y, scale.z}) * collider->bbox.radius;
    }
}