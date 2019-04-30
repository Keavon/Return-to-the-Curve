#pragma once

#include <glm/glm.hpp>

bool intersectRayCylinder(const glm::vec3 &orig, const glm::vec3 &dir, const glm::vec3 &v0, const glm::vec3 &v1,
    float radius, glm::vec3 &position, glm::vec3 &normal);