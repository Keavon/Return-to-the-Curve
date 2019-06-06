#pragma once

#include "WindowManager.h"
#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>

using namespace glm;
using namespace std;

class Pathing
{
public:
    Pathing();
    Pathing(std::vector<glm::vec3> ctrlPts);
    ~Pathing();
    void init();
    void calcBezierCurveTarget(float t);
    void calcCircPos(float theta, float radius, char axis, float axisValue);
    std::vector<glm::vec3> calcLinearPath(glm::vec3 point1, glm::vec3 point2);
	glm::vec3 getTargetPos();
    // Pathing data
    float t;
    glm::vec3 targetPos;
    std::vector<glm::vec3> controlPoints;
};