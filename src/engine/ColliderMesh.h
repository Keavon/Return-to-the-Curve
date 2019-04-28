#pragma once

#include "Collider.h"
#include "ColliderSphere.h"

#include "../Shape.h"
#include <glm/glm.hpp>
#include <memory>

class ColliderMesh : public Collider
{
public:
    ColliderMesh(glm::vec3 *position, glm::quat *orientation, std::shared_ptr<Shape> mesh);

    virtual bool checkCollision(Collider *col);
    virtual bool checkCollision(ColliderMesh *col);
    virtual bool checkCollision(ColliderSphere *col);

    std::shared_ptr<Shape> mesh;
};