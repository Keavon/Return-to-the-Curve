#pragma once

#include "Collider.h"
#include "../Shape.h"
#include <glm/glm.hpp>
#include <memory>

class ColliderMesh : public Collider
{
public:
    ColliderMesh();
    ColliderMesh(std::shared_ptr<Shape> mesh);
    std::shared_ptr<Shape> mesh;
};