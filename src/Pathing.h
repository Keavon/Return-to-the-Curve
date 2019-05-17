#pragma once

#include "WindowManager.h"
#include <glm/glm.hpp>

class Pathing
{
public:
    Pathing();
    ~Pathing();
    glm::vec3 calcBezierCurve(glm::vec3 controlPtA, glm::vec3 controlPtB, glm::vec3 controlPtC, glm::vec3 controlPtD, float t);
	// Pathing data
};