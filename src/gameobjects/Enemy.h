#pragma once

#include "../engine/PhysicsObject.h"
#include "../Shape.h"
#include "../WindowManager.h"
#include <memory>
#include <glm/glm.hpp>

class Enemy : public PhysicsObject
{
public:
    Enemy(float radius, glm::vec3 position);
    void init(std::shared_ptr<Shape> model, WindowManager *windowManager);
    void update(float dt, glm::vec3 dolly, glm::vec3 strafe);

	WindowManager *windowManager;
    float radius;
    float moveSpeed;
    glm::vec3 direction;
};