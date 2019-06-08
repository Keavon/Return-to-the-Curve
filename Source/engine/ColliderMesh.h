#pragma once

#include <glm/glm.hpp>
#include <memory>

#include "Collider.h"
#include "ColliderSphere.h"
#include "PhysicsObject.h"
#include "BoundingBox.h"
#include "../Shape.h"

class ColliderMesh : public Collider
{
public:
    ColliderMesh(shared_ptr<Shape> mesh);

    virtual void checkCollision(PhysicsObject *owner, PhysicsObject *obj, Collider *col);
    virtual void checkCollision(PhysicsObject *owner, PhysicsObject *obj, ColliderSphere *col);
    virtual float getRadius(vec3 scale);

    shared_ptr<Shape> mesh;
};