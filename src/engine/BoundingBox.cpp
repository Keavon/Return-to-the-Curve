#include "BoundingBox.h"

#include <glm/glm.hpp>
#include <algorithm>

using namespace glm;

BoundingBox::BoundingBox(vec3 min, vec3 max) :
    min(min), max(max), radius(std::max(length(min), length(max)))
{
}

BoundingBox::BoundingBox(float radius) :
    min(-radius), max(radius), radius(radius)
{
}