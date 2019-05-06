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

    for (Collision collision : collider->pendingCollisions)
    {
        // resolve collision
        PhysicsObject *other = collision.other;
        vec3 relVel = other->velocity - velocity;
        float velAlongNormal = dot(relVel, collision.normal);
        if (velAlongNormal < 0)
        {
            float e = std::min(other->elasticity, elasticity);
            float j = (-(1 + e) * velAlongNormal) / (invMass + other->invMass);
            vec3 colImpulse = j * collision.normal;
            velocity -= invMass * colImpulse;

            // correct position to prevent sinking/jitter
            if (other->invMass == 0)
            {
                float percent = 0.2;
                float slop = 0.01;
                vec3 correction = std::max(collision.penetration - slop, 0.0f) / (invMass + other->invMass) * percent * -collision.normal;
                position += invMass * correction;
            }
        }
    }
    collider->pendingCollisions.clear();





    netForce.y += GRAVITY * mass;
    velocity += impulse;
    impulse = vec3(0);

    // apply force
    acceleration = netForce / mass;
    velocity += acceleration * dt;
    position += velocity * dt;

    netForce = vec3(0);


    /*
    // Resolve collision
    if (collider->pendingCollision.hit)
    {
        position = collider->pendingCollision.position;
        velocity -= (1.0f + elasticity) * proj(velocity, collider->pendingCollision.normal);
        position += velocity * collider->pendingCollision.t;
        vec3 normalForce = proj(netForce, collider->pendingCollision.normal);
        netForce += normalForce;

        // friction
        vec3 frictionDir = -(velocity - proj(velocity, collider->pendingCollision.normal));
        if (frictionDir != vec3(0))
        {
            frictionDir = normalize(frictionDir);
            netForce += length(normalForce) * friction * frictionDir;
        }
    }
    collider->pendingCollision = {};

    // drag
    if (velocity != vec3(0))
    {
        netForce += powf(length(velocity), 2) * -normalize(velocity) * DRAG_COEFFICIENT;
    }

    velocity += impulse;
    */
}

void PhysicsObject::checkCollision(PhysicsObject *other)
{
    collider->checkCollision(this, other, other->collider.get());
}