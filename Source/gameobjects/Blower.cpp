#include "Blower.h"

using namespace std;
using namespace glm;


Blower::Blower(vec3 position, quat orientation, float radius, float length) :
    PhysicsObject(position, orientation, nullptr, make_shared<TriggerCylinder>(radius, length)),
    radius(radius), length(length)
{
    force = 100;
}

void Blower::update(float dt)
{
    for (auto collision : collider->pendingCollisions)
    {
        collision.other->applyImpulse(vec3(mat4_cast(orientation) * vec4(0, 1, 0, 0)) * force);
    }
    collider->pendingCollisions.clear();
}