#pragma once

#define _USE_MATH_DEFINES

#include <cmath>
#include <math.h>
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <iostream>

#include "../Program.h"
#include "../Shape.h"
#include "Collider.h"
#include "../MatrixStack.h"

using namespace std;
using namespace glm;

class UIObject
{
public:
	//UIObject();
	//UIObject(vec3 position, vec3 scale, quat orientation, shared_ptr<Shape> model, string imgName, int unit);
	UIObject(vec3 position, vec3 scale, quat orientation, shared_ptr<Shape> model, shared_ptr<Texture> img);
	virtual void update(float dt) {};
	void draw(shared_ptr<Program> prog, shared_ptr<MatrixStack> M, int anim = 0);

	vec3 position;
	vec3 scale;
	quat orientation;
	int material;
	shared_ptr<Shape> model;
	shared_ptr<Texture> img;
	bool inView;
	float t;
};