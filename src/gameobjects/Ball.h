#pragma once

#include "../engine/PhysicsObject.h"
#include "../Shape.h"
#include "../WindowManager.h"
#include <memory>
#include <glm/glm.hpp>

class Ball : public PhysicsObject
{
public:
    Ball(glm::vec3 position, glm::quat orientation, std::shared_ptr<Shape> model, float radius);
    void init(WindowManager *windowManager);
    void update(float dt);

	WindowManager *windowManager;
    float radius;
    float moveSpeed;
};