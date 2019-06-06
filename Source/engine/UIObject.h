#pragma once

#include <glm/glm.hpp>
#include <memory>
#include "../Program.h"
#include "../Shape.h"
#include "Collider.h"
#include "../MatrixStack.h"
#include <string>

class UIObject
{
public:
	//UIObject();
	//UIObject(glm::vec3 position, glm::vec3 scale, glm::quat orientation, std::shared_ptr<Shape> model, std::string imgName, int unit);
	UIObject(glm::vec3 position, glm::vec3 scale, glm::quat orientation, std::shared_ptr<Shape> model, std::shared_ptr<Texture> img);
	virtual void update(float dt) {};
	void draw(std::shared_ptr<Program> prog, std::shared_ptr<MatrixStack> M);

	glm::vec3 position;
	glm::vec3 scale;
	glm::quat orientation;
	int material;
	std::shared_ptr<Shape> model;
	std::shared_ptr<Texture> img;
	bool inView;
	float t;
};