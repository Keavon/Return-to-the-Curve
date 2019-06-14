#pragma once

#include "Shape.h"
#include "Material.h"
#include "PhysicsObject.h"
#include "Instance.h"
#include <memory>

using namespace std;
using namespace glm;

class Prefab {
public:
    string name;
    shared_ptr<Shape> model;
    shared_ptr<Material> material;
    float mass = -1;
    float friction = -1;
    float elasticity = -1;
    vec3 scale = vec3(1.0f, 1.0f, 1.0f);

    Prefab(string name, shared_ptr<Shape> model, shared_ptr<Material> material);

    shared_ptr<Instance> getNewInstance();

    // game object-specific data
    vector<vec3> path;
    shared_ptr<Shape> legModel;
    shared_ptr<Shape> footModel;
};