#pragma once

#include "../engine/PhysicsObject.h"
#include <memory>
#include <glm/glm.hpp>

class Goal : public PhysicsObject
{
public:
    Goal(glm::vec3 position, glm::quat orientation, std::shared_ptr<Shape> model, float radius);
    void update(float dt);

    float radius;
};