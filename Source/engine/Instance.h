#pragma once

#include "Shape.h"
#include "Material.h"
#include "PhysicsObject.h"

using namespace std;
using namespace glm;

class Prefab;

class Instance {
public:
    Prefab& definition;
    shared_ptr<Material> material;
    shared_ptr<PhysicsObject> physicsObject;

    Instance(Prefab& definition);
};