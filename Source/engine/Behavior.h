#pragma once

class Behavior
{
public:
	void start();

	void update();
	void lateUpdate();

	void physicsUpdate();
	void latePhysicsUpdate();
};