#include "GameObject.h"

#include <memory>
#include "../Program.h"
#include "../MatrixStack.h"
#include <glm/glm.hpp>
#include <iostream>

using namespace std;
using namespace glm;

GameObject::GameObject(vec3 position, quat orientation, shared_ptr<Shape> model) :
    position(position), orientation(orientation), model(model), scale(vec3(1))
{
}

GameObject::GameObject()
{
}

void GameObject::draw(shared_ptr<Program> prog, shared_ptr<MatrixStack> M)
{
    if (model != NULL)
    {
        M->pushMatrix();
            M->translate(position);
            M->rotate(orientation);
            M->scale(scale);
            glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
            model->draw(prog);
        M->popMatrix();
    }
}