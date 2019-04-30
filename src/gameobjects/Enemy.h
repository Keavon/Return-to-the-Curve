#pragma once

#include "../engine/PhysicsObject.h"
#include "../Shape.h"
#include "../WindowManager.h"
#include <memory>
#include <glm/glm.hpp>

class Enemy : public PhysicsObject
{
public:
    Enemy(float radius);
    void init(std::shared_ptr<Shape> model, WindowManager *windowManager);
    void update(float dt);

	WindowManager *windowManager;
    float moveSpeed;
    float radius;
    float t = 0;
    float targetX;
    float targetZ;
    bool forward;
    bool pointReached;
    glm::vec3 direction;
    std::vector<glm::vec3> pathCtrlPts;
};