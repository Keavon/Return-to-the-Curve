#pragma once

#include "Shape.h"
#include "Material.h"
#include "PhysicsObject.h"
#include "Instance.h"

class Prefab {
public:
    std::shared_ptr<Shape> model;
    std::shared_ptr<Material> material;

    Prefab(std::shared_ptr<Shape> model, std::shared_ptr<Material> material);

    std::shared_ptr<Instance> getNewInstance();
};