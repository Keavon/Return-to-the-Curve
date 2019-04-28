#include "BoundingBox.h"

#include <glm/glm.hpp>

using namespace glm;

BoundingBox::BoundingBox(vec3 min, vec3 max) : min(min), max(max)
{
}