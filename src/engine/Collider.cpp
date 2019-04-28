#include "Collider.h"

#include <glm/glm.hpp>

using namespace glm;

Collider::Collider(vec3 *position, quat *orientation, vec3 min, vec3 max) :
    position(position), orientation(orientation), bbox(min, max)
{
}