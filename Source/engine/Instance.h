#pragma once

#include "Shape.h"
#include "Material.h"
#include "PhysicsObject.h"
#include "Prefab.h"

class Instance {
public:
    std::shared_ptr<Prefab> definition;
    std::shared_ptr<Shape> model;
    std::shared_ptr<Material> material;
    std::shared_ptr<PhysicsObject> physicsObject;

    Instance(std::shared_ptr<Prefab> definition);
};