#pragma once

#include <glm/glm.hpp>

#include "Trigger.h"

using namespace glm;

class Collider;
class ColliderSphere;
class PhysicsObject;

class TriggerSphere : public Trigger
{
public:
    TriggerSphere(float radius);

    virtual void checkCollision(PhysicsObject *owner, PhysicsObject *obj, Collider *col);
    virtual void checkCollision(PhysicsObject *owner, PhysicsObject *obj, ColliderSphere *col);
    virtual float getRadius(vec3 scale);

    float radius;
};