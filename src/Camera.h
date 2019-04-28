#pragma once

#include "WindowManager.h"
#include "gameobjects/Ball.h"
#include <glm/glm.hpp>

class Camera
{
public:
    Camera(WindowManager *windowManager);
    ~Camera();
    void update(float dt, std::shared_ptr<Ball> ball);
	void init();
	glm::vec3 getDolly();
	glm::vec3 getStrafe();

    float speed = 5;
	float sprintFactor = 3;
	float radius = 1.5;
	float height = 5;
	bool flying = false;
	WindowManager *windowManager;

	// Camera data
    double prevXpos;
	double prevYpos;
	double pitch;
	double yaw;
	float distToBall = 15;
	glm::vec3 eye;
	glm::vec3 lookAtPoint;
	glm::vec3 upVec;
	glm::vec3 dolly;
	glm::vec3 strafe;
};