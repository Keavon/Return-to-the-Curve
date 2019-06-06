#pragma once

#include "Collider.h"
#include "ColliderSphere.h"
#include "PhysicsObject.h"

#include <glm/glm.hpp>

class TriggerCylinder : public Collider
{
public:
    TriggerCylinder(float radius, float length);

    virtual void checkCollision(PhysicsObject *owner, PhysicsObject *obj, Collider *col);
    virtual void checkCollision(PhysicsObject *owner, PhysicsObject *obj, ColliderSphere *col);
    virtual float getRadius(glm::vec3 scale);

    float radius;
    float length;
};