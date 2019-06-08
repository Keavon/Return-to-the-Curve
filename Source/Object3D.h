#pragma once

#include <glm/glm.hpp>
#include <memory>
#include "Program.h"
#include "Shape.h"

using namespace std;
using namespace glm;

class Object3D
{
public:
    Object3D(vec3 position, vec3 direction, float speed, shared_ptr<Shape> model, int material, float radius);
    Object3D();
    ~Object3D();
    void update(float dt);
    void draw(shared_ptr<Program> prog);

    vec3 position;
    vec3 direction;
    float radius;
    float speed;
    int material;
    bool dead = false;
    shared_ptr<Shape> model;
};