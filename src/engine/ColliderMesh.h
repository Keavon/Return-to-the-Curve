#pragma once

#include "Collider.h"
#include "../Shape.h"
#include <glm/glm.hpp>

class ColliderMesh : public Collider
{
public:
    Shape mesh;
};