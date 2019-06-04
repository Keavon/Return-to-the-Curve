#include "Logo.h"

#include <glm/glm.hpp>
#include <memory>
#include <cmath>
#include "../Shape.h"
#include "../engine/ColliderMesh.h"
#include "../engine/PhysicsObject.h"

using namespace glm;
using namespace std;

Logo::Logo(vec3 position, quat orientation, shared_ptr<Shape> model) :
	UIObject(position, orientation, model, make_shared<ColliderMesh>(model))
{

}
