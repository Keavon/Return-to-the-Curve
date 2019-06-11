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
    Pathing(vector<vec3> ctrlPts);
    ~Pathing();
    void init();
    void calcBezierCurveTarget(float t);
    void calcCircPos(float theta, float radius, char axis, float axisValue);
    std::vector<glm::vec3> calcBezierPath(std::vector<glm::vec3> controls, float detail);
    std::vector<glm::vec3> calcLinearPath(glm::vec3 point1, glm::vec3 point2);
    glm::vec3 cubicBezier(vec3 v0, vec3 v1, vec3 v2, vec3 v3, float t);
    glm::vec3 quadBezier(vec3 v0, vec3 v1, vec3 v2, float t);
	glm::vec3 getTargetPos();
    // Pathing data
    float t;
    glm::vec3 targetPos;
    std::vector<glm::vec3> controlPoints;
    std::vector<glm::vec3> pathPoints;
private:
    float quadBezierPoint(float p0, float p1, float p2, float t);
    float cubicBezierPoint(float p0, float p1, float p2, float p3, float t);
    glm::vec3 midPoint(glm::vec3 point1, glm::vec3 point2);
};