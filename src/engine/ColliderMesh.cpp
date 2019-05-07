#include "ColliderMesh.h"
#include "ColliderSphere.h"
#include "Collider.h"
#include "PhysicsObject.h"

#include "BoundingBox.h"
#include <memory>
#include <glm/glm.hpp>
#include "../Shape.h"

using namespace glm;
using namespace std;

ColliderMesh::ColliderMesh(PhysicsObject *parent, shared_ptr<Shape> mesh) :
    Collider(parent, mesh->min, mesh->max), mesh(mesh)
{
}

bool ColliderMesh::checkCollision(Collider *col)
{
    return col->checkCollision(this);
}

bool ColliderMesh::checkCollision(ColliderMesh *col)
{
	return false;
    // not handled
}

bool ColliderMesh::checkCollision(ColliderSphere *col)
{
    return checkSphereMesh(col, this);
}