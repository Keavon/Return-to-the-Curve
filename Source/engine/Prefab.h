#pragma once

#include "Shape.h"
#include "Material.h"
#include "PhysicsObject.h"
#include "Instance.h"

using namespace std;
using namespace glm;

class Prefab {
public:
    std::shared_ptr<Shape> model;
    std::shared_ptr<Material> material;
    float mass = -1;
    float friction = -1;
    float elasticity = -1;

    Prefab(std::shared_ptr<Shape> model, std::shared_ptr<Material> material);

    std::shared_ptr<Instance> getNewInstance();
};