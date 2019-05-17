#include "Pathing.h"

#define _USE_MATH_DEFINES
#include <math.h>;
#include <cmath>;
#include <algorithm>
#include <cstdlib>
using namespace std;
using namespace glm;

Pathing::Pathing()
{
}

Pathing::~Pathing()
{
}

glm::vec3 Pathing::calcBezierCurve(glm::vec3 controlPtA, glm::vec3 controlPtB, 
                          glm::vec3 controlPtC, glm::vec3 controlPtD, float t) {
    vec3 targetPos;
    targetPos.x = pow(1 - t, 3) * controlPtA.x +
                  3*t*pow(1-t,2) * controlPtB.x +
                  3*pow(t,2)*(1-t) * controlPtC.x +
                  pow(t,3) * controlPtD.x;
    targetPos.y = pow(1 - t, 3) * controlPtA.y +
                  3*t*pow(1-t,2) * controlPtB.y +
                  3*pow(t,2)*(1-t) * controlPtC.y +
                  pow(t,3) * controlPtD.y;
    targetPos.z = pow(1 - t, 3) * controlPtA.z +
                  3*t*pow(1-t,2) * controlPtB.z +
                  3*pow(t,2)*(1-t) * controlPtC.z +
                  pow(t,3) * controlPtD.z;
    return targetPos;
}
