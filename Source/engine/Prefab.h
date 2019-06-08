#pragma once

#include "Shape.h"
#include "Material.h"
#include "PhysicsObject.h"
#include "Instance.h"

using namespace std;
using namespace glm;

class Prefab {
public:
    shared_ptr<Shape> model;
    shared_ptr<Material> material;
    float mass = -1;
    float friction = -1;
    float elasticity = -1;

    Prefab(shared_ptr<Shape> model, shared_ptr<Material> material);

    shared_ptr<Instance> getNewInstance();
};