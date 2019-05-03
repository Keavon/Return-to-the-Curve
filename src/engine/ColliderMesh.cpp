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

ColliderMesh::ColliderMesh(shared_ptr<Shape> mesh) :
    Collider(mesh->min, mesh->max), mesh(mesh)
{
}

void ColliderMesh::checkCollision(PhysicsObject *owner, PhysicsObject *obj, Collider *col)
{
    col->checkCollision(obj, owner, this);
}

void ColliderMesh::checkCollision(PhysicsObject *owner, PhysicsObject *obj, ColliderMesh *col)
{
    // not implemented
}

void ColliderMesh::checkCollision(PhysicsObject *owner, PhysicsObject *obj, ColliderSphere *col)
{
    checkSphereMesh(obj, col, owner, this);
}