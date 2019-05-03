#include "Box.h"

#include <glm/glm.hpp>
#include <memory>
#include <cmath>
#include "../Shape.h"
#include "../engine/ColliderMesh.h"
#include "../engine/PhysicsObject.h"

using namespace glm;
using namespace std;

Box::Box(vec3 position, quat orientation, shared_ptr<Shape> model) :
    PhysicsObject(position, orientation, model, make_shared<ColliderMesh>(model))
{
    mass = 0;
    invMass = 0;
    elasticity = 0;
    velocity = vec3(0);
}
