#include "UIObject.h"

#define _USE_MATH_DEFINES
#include <cmath>
#include "math.h"

#include <memory>
#include "../Program.h"
#include "../MatrixStack.h"
#include <glm/glm.hpp>
#include <iostream>

using namespace std;
using namespace glm;

UIObject::UIObject(vec3 position, vec3 scale,  quat orientation, shared_ptr<Shape> model, string imgName)
{
	this->position = position;
	this->scale = scale;
	this->orientation = orientation;
	this->position = position;
	this->model = model;
	//model->init();
	inView = true;
	img = make_shared<Texture>();
	img->setFilename(imgName);
	img->init();
	img->setUnit(1);
	img->setWrapModes(GL_MIRRORED_REPEAT, GL_MIRRORED_REPEAT);
	//img = imgTemp;
	cout << "object initialized" << endl;
}

void UIObject::draw(shared_ptr<Program> prog, shared_ptr<MatrixStack> M)
{
	prog->bind();
	M->pushMatrix();
	//M->rotate(quat(0.0f, 1.0f, 0, M_PI_2));
	M->scale(scale);
	M->translate(position);
	glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
	M->loadIdentity();
	model->draw(prog);
	M->popMatrix();
	prog->unbind();
}