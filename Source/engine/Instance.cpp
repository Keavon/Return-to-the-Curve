#pragma once

#include "Instance.h"
#include "Prefab.h"

Instance::Instance(Prefab& definition) : definition(definition)
{
    this->material = definition.material;
    this->physicsObject = make_shared<PhysicsObject>(vec3(0, 0, 0), definition.model);
}
