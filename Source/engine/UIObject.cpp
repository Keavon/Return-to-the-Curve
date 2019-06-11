#include "UIObject.h"

UIObject::UIObject(vec3 position, vec3 scale,  quat orientation, shared_ptr<Shape> model, shared_ptr<Texture> img)
{
	this->position = position;
	this->scale = scale;
	this->orientation = orientation;
	this->position = position;
	this->model = model;
	this->img = img;
	inView = true;
	t = 0;
}

void UIObject::draw(shared_ptr<Program> prog, shared_ptr<MatrixStack> M, int anim)
{
	t += 0.05f;
	prog->bind();
	img->bind(prog->getUniform("Texture"));
	M->pushMatrix();
	M->translate(position);
	
	if (anim == 1) M->rotate(rotate(quat(1, 0, 0, 0), sin(t), vec3(0, 1, 0)));

	if (anim == 2 && t < scale.x) M->scale(vec3(t, t/2.0f, 0.0f));
	else M->scale(scale);

	glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
	M->loadIdentity();
	model->draw(prog);
	M->popMatrix();
	img->unbind();
	prog->unbind();
}