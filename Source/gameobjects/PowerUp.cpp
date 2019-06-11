#include "PowerUp.h"

#include "../Shape.h"
#include "../engine/ColliderSphere.h"
#include "../engine/PhysicsObject.h"
#include "Ball.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <memory>
#include <iostream>

using namespace std;
using namespace glm;


PowerUp::PowerUp(vec3 position, int powerUpType, quat orientation, shared_ptr<Shape> model, float radius, float length) :
    PhysicsObject(position, orientation, model, make_shared<ColliderSphere>(radius)),
    radius(radius), length(length), powerUpType(powerUpType)
{

}

void PowerUp::update()
{
    for (auto collision : collider->pendingCollisions)
    {
        if (dynamic_cast<Ball *>(collision.other) != NULL)
        {
            //cout << "Collide with ball" << endl;
            destroyed = true;
        }
    }
    //cout << "Destroyed: " << destroyed << endl;
}

void PowerUp::init(){
    destroyed = false;
    activatable = true;
}
