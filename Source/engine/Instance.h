#pragma once

#include "Shape.h"
#include "Material.h"
#include "PhysicsObject.h"

class Prefab;

class Instance {
public:
    Prefab& definition;
    std::shared_ptr<Material> material;
    std::shared_ptr<PhysicsObject> physicsObject;

    Instance(Prefab& definition);
};