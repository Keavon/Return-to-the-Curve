#pragma once

#include "../engine/PhysicsObject.h"
#include "../Shape.h"
#include "../WindowManager.h"
#include <memory>
#include <glm/glm.hpp>

class Enemy : public PhysicsObject
{
public:
<<<<<<< Updated upstream
    Enemy(float radius);
    void init(std::shared_ptr<Shape> model, WindowManager *windowManager);
=======
    Enemy(glm::vec3 position, glm::quat orientation, std::shared_ptr<Shape> model, float radius);
    void init(WindowManager *windowManager);
>>>>>>> Stashed changes
    void update(float dt);

	WindowManager *windowManager;
    float moveSpeed;
    float radius;
    float t;
    float targetX;
    float targetZ;
    bool forward;
    bool pointReached;
    glm::vec3 direction;
    std::vector<glm::vec3> pathCtrlPts;
};