#pragma once

#include "WindowManager.h"
#include "gameobjects/Ball.h"
#include <glm/glm.hpp>

class Player
{
public:
    Player(WindowManager *windowManager);
    ~Player();
    void update(float dt, std::shared_ptr<Ball> ball);
	void init();

    float speed = 5;
	float sprintFactor = 3;
	float radius = 1.5;
	float height = 2;
	bool flying = false;
	WindowManager *windowManager;

	// Camera data
    double prevXpos;
	double prevYpos;
	double pitch;
	double yaw;
	glm::vec3 eye;
	glm::vec3 lookAtPoint;
	glm::vec3 upVec;

};