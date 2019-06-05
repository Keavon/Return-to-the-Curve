#pragma once

#include "../engine/PhysicsObject.h"
#include <memory>
#include <glm/glm.hpp>

class Blower : public PhysicsObject
{
public:
    Blower(glm::vec3 position, glm::quat orientation, float radius, float length);
    void update(float dt);

    float radius;
    float length;
    float force;
};