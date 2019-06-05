#include "Blower.h"
#include "../engine/TriggerSphere.h"
#include "Ball.h"

#include <glm/glm.hpp>
#include <memory>
#include <iostream>

using namespace std;
using namespace glm;


<<<<<<< HEAD
Blower::Blower(vec3 position, quat orientation, shared_ptr<Shape> model, float radius, float length) :
    PhysicsObject(position, orientation, vec3(1, 1, 1), model, make_shared<TriggerCylinder>(radius, length)), radius(radius), length(length)
=======
Blower::Blower(vec3 position, quat orientation, float radius, float length) :
    PhysicsObject(position, orientation, nullptr, make_shared<TriggerCylinder>(radius, length)),
    radius(radius), length(length)
>>>>>>> fan
{
    force = 100;
}

void Blower::update(float dt)
{
    for (auto collision : collider->pendingCollisions)
    {
        collision.other->impulse += vec3(mat4_cast(orientation) * vec4(0, 1, 0, 0)) * force;
    }
    collider->pendingCollisions.clear();
}