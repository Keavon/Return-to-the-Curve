#pragma once

#include "../engine/PhysicsObject.h"
#include "../Shape.h"
#include "../WindowManager.h"
#include <memory>
#include <glm/glm.hpp>

class Enemy : public PhysicsObject
{
public:
    Enemy(std::vector<glm::vec3> enemyPath, glm::quat orientation, std::shared_ptr<Shape> model, float radius);
    void init(WindowManager *windowManager);
    void update(float dt);

	WindowManager *windowManager;
    float moveSpeed;
    float radius;
    float t = 0.1;
    float targetX;
    float targetZ;
    float targetY;
    bool forward;
    bool pointReached;
    glm::vec3 direction;
    std::vector<glm::vec3> pathCtrlPts;
};