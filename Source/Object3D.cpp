#include "Object3D.h"

Object3D::Object3D(vec3 position, vec3 direction, float speed, shared_ptr<Shape> model, int material, float radius) :
    position(position), direction(direction), speed(speed), model(model), material(material), radius(radius)
{
}

Object3D::Object3D()
{
}

Object3D::~Object3D()
{
}

void Object3D::update(float dt)
{
    position += direction * speed * dt;
}

void Object3D::draw(shared_ptr<Program> prog)
{
    model->draw(prog);
}