#include "PhysicsObject.h"

#include "ColliderSphere.h"
#include "Collider.h"

#define NOMINMAX
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/projection.hpp>
#include <glm/gtx/intersect.hpp>
#include <glm/glm.hpp>
#include <memory>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <unordered_set>

using namespace std;
using namespace glm;

bool inRange(float n, float low, float high)
{
    return low <= n && n <= high;
}

PhysicsObject::PhysicsObject(vec3 position, quat orientation,
                             shared_ptr<Shape> model, shared_ptr<Collider> collider) : GameObject(position, orientation, model), collider(collider),
                                                                                       netForce(0), impulse(0), acceleration(0), velocity(0), mass(0), invMass(0),
                                                                                       normForce(0), friction(0), elasticity(0), speed(0)
{
}
void PhysicsObject::update(float dt)
{
    normForce = vec3(0);
    netForce.y += GRAVITY * mass;

    // filter collisions so that objects don't bump over edges
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

    float maxImpact = -1;
    Collision maxImpactCollision;
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

            if (fabs(velAlongNormal) > maxImpact)
            {
                maxImpact = fabs(velAlongNormal);
                maxImpactCollision = collision;
            }
        }
    }
    if (maxImpact > 0)
    {
        onHardCollision(maxImpact, maxImpactCollision);
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
    else if (scale == vec3(0))
    {
        return collider->bbox.radius;
    }
    else
    {
        return (std::max)({scale.x, scale.y, scale.z}) * collider->bbox.radius;
    }
}

vec3 PhysicsObject::getCenterPos()
{
    if (collider == NULL || collider->bbox.center == vec3(0))
    {
        return position;
    }
    else if (orientation == quat(1, 0, 0, 0))
    {
        return position + collider->bbox.center * scale;
    }
    else
    {
        return position + vec3(mat4_cast(orientation) * vec4(collider->bbox.center * scale, 1));
    }
}

// Called when the object hits something with a normal velocity > 0
void PhysicsObject::onHardCollision(float impactVel, Collision &collision)
{

}