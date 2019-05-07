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
    void update(float dt, glm::vec3 dolly, glm::vec3 strafe);

	WindowManager *windowManager;
    float radius;
    float moveForce;
    bool holdingJump;
};