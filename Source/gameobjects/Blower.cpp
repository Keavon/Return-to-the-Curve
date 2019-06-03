#include "Blower.h"
#include "../engine/TriggerSphere.h"
#include "Ball.h"

#include <glm/glm.hpp>
#include <memory>
#include <iostream>

using namespace std;
using namespace glm;


Blower::Blower(vec3 position, quat orientation, shared_ptr<Shape> model, float radius, float length) :
    PhysicsObject(position, orientation, vec3(1, 1, 1), model, make_shared<TriggerCylinder>(radius, length)), radius(radius), length(length)
{

}

void Blower::update(float dt)
{
    for (auto collision : collider->pendingCollisions)
    {
        collision.other->netForce += force;
    }
}