#pragma once

#include <glm/glm.hpp>

using namespace std;
using namespace glm;

class Frustum
{
public:
    void extractPlanes(glm::mat4 P, glm::mat4 V);
    bool checkAABB(const glm::vec3 &mins, const glm::vec3 &maxs);
    bool checkSphere(const glm::vec3 &center, float radius);

    glm::vec4 planes[6];
};