#include "BoundingBox.h"

#include <glm/glm.hpp>
#include <algorithm>

using namespace glm;

BoundingBox::BoundingBox(vec3 min, vec3 max) :
    min(min), max(max), radius(std::max(length(min), length(max)))
{
}

float BoundingBox::getRadius(vec3 scale)
{
    return std::max({scale.x, scale.y, scale.z}) * radius;
}