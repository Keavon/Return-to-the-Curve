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
    bool forward;
    bool pointReached;
    glm::vec3 direction;
    std::vector<glm::vec3> pathCtrlPts;
	PhysicsObject uleg1, uleg2, uleg3, uleg4, lleg1, lleg2, lleg3, lleg4, foot1, foot2, foot3, foot4;
};