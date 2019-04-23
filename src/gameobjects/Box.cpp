#include "Box.h"

#include <glm/glm.hpp>
#include <memory>
#include "../Shape.h"
#include "../engine/ColliderMesh.h"

using namespace glm;
using namespace std;

Box::Box(vec3 position)
{
    this->position = position;
    orientation = quat(1, 0, 0, 0);
}

void Box::init(shared_ptr<Shape> model)
{
    this->model = model;
    collider = ColliderMesh(model);
}