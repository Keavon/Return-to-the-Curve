#pragma once

#include <glm/glm.hpp>

using namespace std;
using namespace glm;

class Frustum
{
public:
    void extractPlanes(mat4 P, mat4 V);
    bool checkAABB(const vec3 &mins, const vec3 &maxs);
    bool checkSphere(const vec3 &center, float radius);

    vec4 planes[6];
};