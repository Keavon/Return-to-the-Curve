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
    Object3D(glm::vec3 position, glm::vec3 direction, float speed, std::shared_ptr<Shape> model, int material, float radius);
    Object3D();
    ~Object3D();
    void update(float dt);
    void draw(std::shared_ptr<Program> prog);

    glm::vec3 position;
    glm::vec3 direction;
    float radius;
    float speed;
    int material;
    bool dead = false;
    std::shared_ptr<Shape> model;
};