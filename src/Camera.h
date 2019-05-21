#pragma once

#include "Pathing.h"
#include "WindowManager.h"
#include "gameobjects/Ball.h"
#include <glm/glm.hpp>

class Camera
{
public:
    Camera(WindowManager *windowManager, glm::vec3 centerOfLevel);
    ~Camera();
    void update(float dt, std::shared_ptr<Ball> ball);
	void init();
	void startLvlPreview(glm::vec3 lvlCenterPt);
	glm::vec3 getDolly();
	glm::vec3 getStrafe();

    float speed = 5;
	float sprintFactor = 3;
	float radius = 1.5;
	float height = 5;
	bool flying = false;
	bool previewLvl = false;
	Pathing *cameraPath;
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

	//Camera Path Data
	bool pointReached;
	float lookAtPathSpeed;
	float pathRadius = 100;
	float pathSpeed;
	float pathT;
	double prevTime;
	float startTimer;
	float theta;
	
	glm::vec3 cameraPathTarget;
	glm::vec3 cameraStartPos;
	glm::vec3 circStartPos;
	glm::vec3 circNextPos;
	glm::vec3 levelCenterPoint;
	glm::vec3 lookAtPathVel;
	glm::vec3 lookAtTarget;
	glm::vec3 lookAtVec;
	glm::vec3 pathVect;
	glm::vec3 pathVel;
};