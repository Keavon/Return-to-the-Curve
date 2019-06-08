#pragma once

#include <glm/glm.hpp>

#include "Collider.h"
#include "ColliderSphere.h"
#include "ColliderMesh.h"
#include "BoundingBox.h"
#include "PhysicsObject.h"

using namespace glm;

class TriggerSphere : public Collider
{
public:
    TriggerSphere(float radius);

    virtual void checkCollision(PhysicsObject *owner, PhysicsObject *obj, Collider *col);
    virtual void checkCollision(PhysicsObject *owner, PhysicsObject *obj, ColliderSphere *col);
    virtual float getRadius(vec3 scale);

    float radius;
};