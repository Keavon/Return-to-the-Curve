#pragma once

#define _USE_MATH_DEFINES

#include <cmath>
#include <math.h>
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <cstdlib>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>

#include "WindowManager.h"

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
	glm::vec3 getTargetPos();
    // Pathing data
    float t;
    glm::vec3 targetPos;
    std::vector<glm::vec3> controlPoints;
};