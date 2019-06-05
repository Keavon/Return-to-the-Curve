#pragma once

#include "../engine/PhysicsObject.h"
#include "../engine/TriggerSphere.h"
#include "Ball.h"

#include <glm/glm.hpp>
#include <memory>
#include <iostream>

class Blower : public PhysicsObject
{
public:
    Blower(glm::vec3 position, glm::quat orientation, float radius, float length);
    void update(float dt);

    float radius;
    float length;
    float force;
};