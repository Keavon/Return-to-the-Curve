#pragma once

#include <memory>
#include <string.h>
#include <glm/glm.hpp>
#include <iostream>

#include "../Shape.h"
#include "../engine/PhysicsObject.h"
#include "../engine/ColliderSphere.h"
#include "../engine/PhysicsObject.h"
#include "../engine/TriggerSphere.h"
#include "Ball.h"
#include "../engine/Time.h"

#define GLM_ENABLE_EXPERIMENTAL

using namespace std;
using namespace glm;

class PowerUp : public PhysicsObject
{
public:
    PowerUp(vec3 position, quat orientation, shared_ptr<Shape> model, float radius, string powerUpType);
    virtual void update();
    void init();

    float hiddenTime = 0;
    float radius;
    float length;
    string powerUpType;
    vec3 force;
    virtual void triggerEnter(PhysicsObject *object);
};