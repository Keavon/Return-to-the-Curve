#include "Pathing.h"

#define _USE_MATH_DEFINES
#include <cmath>
#include <math.h>
#include <algorithm>
#include <cstdlib>

Pathing::Pathing(std::vector<glm::vec3> ctrlPts):
    controlPoints(ctrlPts)
{
    targetPos = vec3(0);
}

Pathing::~Pathing()
{
}

void Pathing::calcBezierCurve(float t) {
    if (0 <= t && t <= 1) {
        targetPos.x = pow(1 - t, 3) * controlPoints[0].x +
                    3*t*pow(1-t,2) * controlPoints[1].x +
                    3*pow(t,2)*(1-t) * controlPoints[2].x +
                    pow(t,3) * controlPoints[3].x;
        targetPos.y = pow(1 - t, 3) * controlPoints[0].y +
                    3*t*pow(1-t,2) * controlPoints[1].y +
                    3*pow(t,2)*(1-t) * controlPoints[2].y +
                    pow(t,3) * controlPoints[3].y;
        targetPos.z = pow(1 - t, 3) * controlPoints[0].z +
                    3*t*pow(1-t,2) * controlPoints[1].z +
                    3*pow(t,2)*(1-t) * controlPoints[2].z +
                    pow(t,3) * controlPoints[3].z;
    }
}

glm::vec3 Pathing::getTargetPos() {
    return targetPos;
}