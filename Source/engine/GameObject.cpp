#include "GameObject.h"

GameObject::GameObject(vec3 position, shared_ptr<Shape> model) : GameObject::GameObject(position, quat(1, 0, 0, 0), vec3(1, 1, 1), model) {}

GameObject::GameObject(vec3 position, quat orientation, shared_ptr<Shape> model) : GameObject::GameObject(position, orientation, vec3(1, 1, 1), model) {}

GameObject::GameObject(vec3 position, quat orientation, vec3 scale, shared_ptr<Shape> model)
{
    this->position = position;
    this->orientation = orientation;
    this->scale = scale;
    this->model = model;
    this->inView = true;
    this->hidden = false;
}

void GameObject::draw(shared_ptr<Program> prog, shared_ptr<MatrixStack> M)
{
    if (model != NULL && (inView || !cull) && !hidden)
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

bool GameObject::cull = false;

void GameObject::setCulling(bool cull)
{
    GameObject::cull = cull;
}