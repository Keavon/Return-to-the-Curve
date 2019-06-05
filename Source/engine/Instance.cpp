#pragma once

#include "Instance.h"

using namespace std;
using namespace glm;

Instance::Instance(shared_ptr<Prefab> definition)
{
    this->definition = definition;
    this->model = definition->model;
    this->material = definition->material;
    this->physicsObject = make_shared<PhysicsObject>(vec3(0, 0, 0), definition->model);
}
