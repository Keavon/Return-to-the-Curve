#pragma once

#include "Collider.h"
#include "ColliderSphere.h"
#include "PhysicsObject.h"

#include "../Shape.h"
#include <glm/glm.hpp>
#include <memory>

class ColliderMesh : public Collider
{
public:
    ColliderMesh(std::shared_ptr<Shape> mesh);

    virtual void checkCollision(PhysicsObject *owner, PhysicsObject *obj, Collider *col);
    virtual void checkCollision(PhysicsObject *owner, PhysicsObject *obj, ColliderSphere *col);
    virtual float getRadius(glm::vec3 scale);

    std::shared_ptr<Shape> mesh;
};