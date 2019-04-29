#pragma once

#include "Collider.h"
#include "ColliderMesh.h"
#include "PhysicsObject.h"

#include <glm/glm.hpp>

class ColliderSphere : public Collider
{
public:
    ColliderSphere(PhysicsObject *parent, float radius);

    virtual bool checkCollision(Collider *col);
    virtual bool checkCollision(ColliderMesh *col);
    virtual bool checkCollision(ColliderSphere *col);

    float radius;
};