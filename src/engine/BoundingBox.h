#pragma once

#include <glm/glm.hpp>

class BoundingBox
{
public:
    BoundingBox(glm::vec3 min, glm::vec3 max);
    float getRadius(glm::vec3 scale);

    glm::vec3 min;
    glm::vec3 max;
    float radius;
};