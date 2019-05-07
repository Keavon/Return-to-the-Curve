#pragma once

#include "Collider.h"
#include "ColliderSphere.h"
#include "PhysicsObject.h"

#include <glm/glm.hpp>

class TriggerSphere : public Collider
{
public:
    TriggerSphere(float radius);

    virtual void checkCollision(PhysicsObject *owner, PhysicsObject *obj, Collider *col);
    virtual void checkCollision(PhysicsObject *owner, PhysicsObject *obj, ColliderSphere *col);

    float radius;
};