#include "GameObject.h"

#include <memory>
#include "../Program.h"
#include "../MatrixStack.h"
#include <glm/glm.hpp>

using namespace std;
using namespace glm;

GameObject::GameObject(vec3 position, quat orientation, shared_ptr<Shape> model, int material) :
    position(position), orientation(orientation), model(model), material(material)
{
}

GameObject::GameObject()
{
}

GameObject::~GameObject()
{
}

void GameObject::update(float dt)
{
    // position += direction * speed * dt;
}

void GameObject::draw(shared_ptr<Program> prog, shared_ptr<MatrixStack> M)
{
    M->pushMatrix();
        M->translate(position);
        M->rotate(orientation);
        glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
        model->draw(prog);
    M->popMatrix();
}