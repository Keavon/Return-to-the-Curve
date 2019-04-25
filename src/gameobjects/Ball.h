#pragma once

#include "../engine/PhysicsObject.h"
#include "../Shape.h"
#include "../WindowManager.h"
#include <memory>
#include <glm/glm.hpp>

class Ball : public PhysicsObject
{
public:
    Ball(float radius, glm::vec3 position);
    void init(std::shared_ptr<Shape> model, WindowManager *windowManager);
    void update(float dt);

	WindowManager *windowManager;
    float radius;
    float moveSpeed;
    glm::vec3 direction;
};