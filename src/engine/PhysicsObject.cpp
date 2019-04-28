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

PhysicsObject::PhysicsObject(vec3 position, quat orientation,
    shared_ptr<Shape> model, shared_ptr<Collider> collider) :
    GameObject(position, orientation, model), collider(collider)
{
}
