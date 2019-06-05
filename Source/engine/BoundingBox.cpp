#include "BoundingBox.h"

#include <glm/glm.hpp>
#include <algorithm>

using namespace glm;

BoundingBox::BoundingBox(vec3 min, vec3 max) :
    min(min), max(max), center((min+max)/2.0f), radius(length(max-min)/2)
{
}

BoundingBox::BoundingBox(float radius) :
    min(-radius), max(radius), radius(radius), center(0)
{
}