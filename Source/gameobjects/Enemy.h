#pragma once

#define _USE_MATH_DEFINES

#include <memory>
#include <glm/glm.hpp>
#include <cmath>
#include <math.h>

#include "../engine/PhysicsObject.h"
#include "../engine/ColliderSphere.h"
#include "../Pathing.h"
#include "../Shape.h"
#include "../WindowManager.h"
#include "../engine/Time.h"

class Enemy : public PhysicsObject
{
public:
	Enemy(std::vector<glm::vec3> enemyPath, glm::quat orientation, std::shared_ptr<Shape> model, 
		std::shared_ptr<Shape> legmodel, std::shared_ptr<Shape> footmodel, float radius);
    void init(WindowManager *windowManager);
    void update(vec3 ballPosition);
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
    int state; // 0 - idle 1 - following player 2 - return home 3 - nonSentry
    bool forward;
    bool sentry;
    bool pointReached;
    bool ballInRange;
    glm::vec3 direction;
    glm::vec3 sentryHome;
    std::vector<glm::vec3> sentryIdlePath;
    std::vector<glm::vec3> sentryFollowPath;
    std::vector<glm::vec3> sentryPathHome;
    std::vector<glm::vec3> defaultPath;
	std::shared_ptr<Shape> legModel;
	std::shared_ptr<Shape> footModel;
	//GameObject uleg1, uleg2, uleg3, uleg4, lleg1, lleg2, lleg3, lleg4, foot1, foot2, foot3, foot4;
};