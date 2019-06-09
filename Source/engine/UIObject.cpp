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

//UIObject::UIObject()
//{
//}

//UIObject::UIObject(vec3 position, vec3 scale,  quat orientation, shared_ptr<Shape> model, string imgName, int unit)
UIObject::UIObject(vec3 position, vec3 scale,  quat orientation, shared_ptr<Shape> model, shared_ptr<Texture> img)
{
	this->position = position;
	this->scale = scale;
	this->orientation = orientation;
	this->position = position;
	this->model = model;
	this->img = img;
	//model->init();
	inView = true;
	t = 0;
	//this->img = make_shared<Texture>();
	//this->img->setFilename(imgName);
	//this->img->init();
	//this->img->setUnit(unit);
	//this->img->setWrapModes(GL_MIRRORED_REPEAT, GL_MIRRORED_REPEAT);
	//img = imgTemp;
	cout << "object initialized" << endl;
}

void UIObject::draw(shared_ptr<Program> prog, shared_ptr<MatrixStack> M, int anim)
{
	//img->bind(prog->getAttribute("vertTex"));
	//img->unbind();
	t += 0.05;
	prog->bind();
	img->bind(prog->getAttribute("vertTex"));
	M->pushMatrix();
	M->translate(position);
	if (anim == 2 && t < scale.x) {
		M->scale(vec3(t, t/2.0f, 0.0f));
	}
	else {
		M->scale(scale);
	}
	if (anim == 1) {
		M->rotate(rotate(quat(1, 0, 0, 0), sin(t), vec3(0, 1, 0)));
	}
	glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
	M->loadIdentity();
	model->draw(prog);
	M->popMatrix();
	img->unbind();
	prog->unbind();
}