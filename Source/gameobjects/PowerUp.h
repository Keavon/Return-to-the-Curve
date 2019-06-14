#pragma once

#include "../engine/PhysicsObject.h"
#include <memory>
#include <glm/glm.hpp>

class PowerUp : public PhysicsObject
{
public:
    PowerUp(glm::vec3 position, int powerUpType, glm::quat orientation, std::shared_ptr<Shape> model, float radius);
    void update();
    void init();

    float radius;
    float length;
    int powerUpType; // 0- Jump Height 1- Burst of acceleration  3- 
    glm::vec3 force;
    bool destroyed;
    bool activatable;
};