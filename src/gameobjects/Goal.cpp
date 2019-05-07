#include "Goal.h"
#include "../engine/TriggerSphere.h"
#include "Ball.h"

#include <glm/glm.hpp>
#include <memory>
#include <iostream>

using namespace std;
using namespace glm;

bool ballInGoal;


Goal::Goal(vec3 position, quat orientation, shared_ptr<Shape> model, float radius) :
    PhysicsObject(position, orientation, model, make_shared<TriggerSphere>(radius)),
    radius(radius)
{

}

void Goal::update(float dt)
{
    for (auto collision : collider->pendingCollisions)
    {
        if (static_cast<Ball *>(collision.other) != NULL)
        {
            ballInGoal = true;
        }
    }
}