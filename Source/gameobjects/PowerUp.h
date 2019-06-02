#pragma once

#include "../engine/PhysicsObject.h"
#include <memory>
#include <glm/glm.hpp>

class PowerUp : public PhysicsObject
{
public:
    PowerUp(glm::vec3 position, int powerUpType, glm::quat orientation, std::shared_ptr<Shape> model, float radius, float length);
    void update(float dt);
    void init();

    float radius;
    float length;
    int powerUpType;
    glm::vec3 force;
    bool destroyed;
};