#include "Beam.h"

#include "../Shape.h"
#include "../engine/ColliderMesh.h"
#include <memory>
#include <glm/glm.hpp>

using namespace glm;
using namespace std;

Beam::Beam(vec3 position, quat orientation, std::shared_ptr<Shape> model) :
    PhysicsObject(position, orientation, model, make_shared<ColliderMesh>(model))
{
    this->ignoreCollision = true;
}