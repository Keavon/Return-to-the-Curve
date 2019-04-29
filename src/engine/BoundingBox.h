#pragma once

#include <glm/glm.hpp>

class BoundingBox
{
public:
    BoundingBox(glm::vec3 min, glm::vec3 max);
    glm::vec3 min;
    glm::vec3 max;
    float radius;
};