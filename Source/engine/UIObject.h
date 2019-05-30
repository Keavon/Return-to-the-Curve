#pragma once

#include <glm/glm.hpp>
#include <memory>
#include "../Program.h"
#include "../Shape.h"
#include "Collider.h"
#include "../MatrixStack.h"

class UIObject
{
public:
	UIObject(glm::vec3 position, glm::quat orientation, std::shared_ptr<Shape> model);
	UIObject();
	virtual void update(float dt) {};
	void draw(std::shared_ptr<Program> prog, std::shared_ptr<MatrixStack> M);
	void drawPers(std::shared_ptr<Program> prog, std::shared_ptr<MatrixStack> M);

	glm::vec3 position;
	glm::quat orientation;
	int material;
	glm::vec3 scale;
	std::shared_ptr<Shape> model;
	bool inView;
};