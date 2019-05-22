#pragma once

#include "../engine/PhysicsObject.h"
#include "../engine/ColliderSphere.h"
#include "../Pathing.h"
#include "../Shape.h"
#include "../WindowManager.h"
#include <memory>
#include <glm/glm.hpp>
#include <cmath>

class Enemy : public PhysicsObject
{
public:
	Enemy(std::vector<glm::vec3> enemyPath, glm::quat orientation, std::shared_ptr<Shape> model, 
		std::shared_ptr<Shape> legmodel, std::shared_ptr<Shape> footmodel, float radius);
    void init(WindowManager *windowManager);
    void update(float dt);
	void draw(std::shared_ptr<Program> prog, std::shared_ptr<MatrixStack> M); 
	MatrixStack setlLeg(MatrixStack uLeg, float offset, float t);
	MatrixStack setFoot(MatrixStack lLeg, float offset, float t);
    Pathing *curvePath;
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
	std::shared_ptr<Shape> legModel;
	std::shared_ptr<Shape> footModel;
	//GameObject uleg1, uleg2, uleg3, uleg4, lleg1, lleg2, lleg3, lleg4, foot1, foot2, foot3, foot4;
};