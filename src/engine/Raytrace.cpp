#include "Raytrace.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/projection.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <glm/glm.hpp>
#include <cmath>
#include <algorithm>
#include <iostream>

using namespace glm;
using namespace std;


// https://www.cl.cam.ac.uk/teaching/1999/AGraphHCI/SMAG/node2.html#eqn:rectray

// assumed that dir is normalized
bool intersectRayCylinder(const vec3 &orig, const vec3 &dir, const vec3 &v0, const vec3 &v1, float radius,
    vec3 &position, vec3 &normal)
{
    // Create matrix to transform cylinder to origin
    mat4 T;

    T = lookAt(v0, v1, vec3(0, 1, 0));
    if (isnan(length2(T * vec4(1))))
    {
        if (v0.y < v1.y)
        {
            T = rotate(mat4(1), (float) -M_PI_2, vec3(1, 0, 0)) * translate(mat4(1), -v0);
        }
        else
        {
            T = rotate(mat4(1), (float) -M_PI_2, vec3(1, 0, 0)) * translate(mat4(1), -v1);
        }
    }
    vec3 dir_T = vec3(T * vec4(dir, 0.0f));
    vec3 orig_T = vec3(T * vec4(orig, 1.0f));

    // Solve for t using quadratic formula
    float a = pow(dir_T.x, 2) + pow(dir_T.y, 2);
    float b = 2 * (orig_T.x * dir_T.x + orig_T.y * dir_T.y);
    float c = pow(orig_T.x, 2) + pow(orig_T.y, 2) - radius;

    float discrim = pow(b, 2) - 4 * a * c;

    if (discrim < 0)
    {
        // no collision
        return false;
    }

    float discrim_sqrt = sqrt(discrim);
    float t = std::max((-b + discrim_sqrt) / (2 * a), (-b - discrim_sqrt) / (2 * a));
    if (t < 0)
    {
        return false;
    }

    // Calculate intersection point and normal
    float length = distance(v0, v1);
    vec3 intersection_T = orig_T + dir_T * t;
    if (intersection_T.z > 0 || intersection_T.z < -length)
    {
        return false;
    }
    vec3 normal_T = vec3(intersection_T.x, intersection_T.y, 0.0f) / radius;

    // Transform back to original position
    mat4 T_inv = inverse(T);
    position = orig + dir * t;
    normal = vec3(T_inv * vec4(normal_T, 0.0f));

    return true;
}