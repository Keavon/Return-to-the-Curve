#include "Prefab.h"

Prefab::Prefab(string name, shared_ptr<Shape> model, shared_ptr<Material> material)
{
    this->name = name;
    this->model = model;
    this->material = material;
}

shared_ptr<Instance> Prefab::getNewInstance()
{
    shared_ptr<Instance> instance = make_shared<Instance>(*this);

    if (mass != -1) instance->physicsObject->setMass(mass);
    if (friction != -1) instance->physicsObject->setFriction(friction);
    if (elasticity != -1) instance->physicsObject->setElasticity(elasticity);

    return instance;
}
