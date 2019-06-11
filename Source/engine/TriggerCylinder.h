#pragma once

#include "Trigger.h"
#include "BoundingBox.h"
#include "PhysicsObject.h"

#include <glm/glm.hpp>

using namespace glm;

class PhysicsObject;
class Collider;
class ColliderSphere;

class TriggerCylinder : public Trigger
{
public:
    TriggerCylinder(float radius, float length);

    virtual void checkCollision(PhysicsObject *owner, PhysicsObject *obj, Collider *col);
    virtual void checkCollision(PhysicsObject *owner, PhysicsObject *obj, ColliderSphere *col);
    virtual float getRadius(vec3 scale);

    float radius;
    float length;
};