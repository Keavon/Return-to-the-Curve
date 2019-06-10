#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <iostream>

#include "../Program.h"
#include "../Shape.h"
#include "../MatrixStack.h"
#include "Collider.h"

using namespace std;
using namespace glm;

class GameObject
{
public:
    GameObject();
    GameObject(vec3 position, shared_ptr<Shape> model);
    GameObject(vec3 position, quat orientation, shared_ptr<Shape> model);
    GameObject(vec3 position, quat orientation, vec3 scale, shared_ptr<Shape> model);
    virtual void update() {};
    virtual void draw(shared_ptr<Program> prog, shared_ptr<MatrixStack> M);
    static void setCulling(bool cull);

    vec3 position;
    quat orientation;
    vec3 scale;
    shared_ptr<Shape> model;
    int material;
    bool inView;

    static bool cull;
};