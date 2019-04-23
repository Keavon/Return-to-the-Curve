#include "ColliderSphere.h"

#include "BoundingBox.h"
#include <glm/glm.hpp>

using namespace glm;

ColliderSphere::ColliderSphere() : radius(0)
{
    bbox = BoundingBox(vec3(0), vec3(0));
}

ColliderSphere::ColliderSphere(float radius) : radius(radius)
{
    bbox = BoundingBox(vec3(-radius), vec3(radius));
}
