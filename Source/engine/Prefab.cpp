#include "Prefab.h"

using namespace std;
using namespace glm;

Prefab::Prefab(shared_ptr<Shape> model, shared_ptr<Material> material)
{
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
