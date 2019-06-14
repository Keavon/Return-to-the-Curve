#pragma once

#include "../engine/PhysicsObject.h"
#include <memory>
#include <string.h>
#include <glm/glm.hpp>

class PowerUp : public PhysicsObject
{
public:
    PowerUp(glm::vec3 position, glm::quat orientation, std::shared_ptr<Shape> model, float radius, string powerUpType);
    void update();
    void init();

    float radius;
    float length;
    string powerUpType;
    glm::vec3 force;
    bool destroyed;
    bool activatable;
};