#include "UIObject.h"

#include <memory>
#include "../Program.h"
#include "../MatrixStack.h"
#include <glm/glm.hpp>
#include <iostream>

using namespace std;
using namespace glm;

UIObject::UIObject(vec3 position, quat orientation, shared_ptr<Shape> model) :
	position(position), orientation(orientation), model(model), scale(vec3(1)), inView(true)
{
}

void UIObject::draw(shared_ptr<Program> prog, shared_ptr<MatrixStack> M)
{
	M->pushMatrix();
	M->scale(vec3(0.125f, 0.5f, 0));
	M->translate(vec3(0.75f, 0.75f, 0));
	glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
	M->loadIdentity();
	glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(M->topMatrix()));
	glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, value_ptr(M->topMatrix()));
	glDrawArrays(GL_TRIANGLES, 0, 6);
	M->popMatrix();
}