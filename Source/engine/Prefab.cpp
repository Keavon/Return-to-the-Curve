#pragma once

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
    return make_shared<Instance>(this);
}
