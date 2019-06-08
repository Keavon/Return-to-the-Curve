#include "ColliderMesh.h"

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

void ColliderMesh::checkCollision(PhysicsObject *owner, PhysicsObject *obj, ColliderSphere *col)
{
    checkSphereMesh(obj, col, owner, this);
}

float ColliderMesh::getRadius(vec3 scale)
{
    return length(scale * (bbox.max - bbox.min)) / 2;
}
