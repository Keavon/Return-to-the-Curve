#include "PhysicsObject.h"

#include "ColliderSphere.h"
#include "Collider.h"

#include <glm/glm.hpp>
#include <memory>
#include <iostream>
#include <cmath>

using namespace std;
using namespace glm;

bool inRange(float n, float low, float high)
{
    return low <= n && n <= high;
}

bool PhysicsObject::collide(shared_ptr<PhysicsObject> other)
{
    if ((other->position.x > position.x + collider.bbox.min.x && other->position.x < position.x + collider.bbox.max.x) &&
        (other->position.z > position.z + collider.bbox.min.z && other->position.z < position.z + collider.bbox.max.z) &&
        (inRange(other->position.y - (position.y + collider.bbox.max.y), 0, fabs(other->collider.bbox.min.y))))
    {
        other->velocity.y = 0;
        other->position.y = position.y + collider.bbox.max.y - other->collider.bbox.min.y;
    }
}