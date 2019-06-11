#pragma once

#include <glm/glm.hpp>
#include <algorithm>

using namespace glm;

class BoundingBox
{
public:
    BoundingBox(vec3 min, vec3 max);
    BoundingBox(float radius);

    vec3 min;
    vec3 max;
    float radius;
    vec3 center;
};