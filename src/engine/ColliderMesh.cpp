#include "ColliderMesh.h"
#include "ColliderSphere.h"
#include "Collider.h"

#include "BoundingBox.h"
#include <memory>
#include <glm/glm.hpp>
#include "../Shape.h"

using namespace glm;
using namespace std;

ColliderMesh::ColliderMesh(vec3 *position, quat *orientation, shared_ptr<Shape> mesh) :
    Collider(position, orientation, mesh->min, mesh->max), mesh(mesh)
{
}

bool ColliderMesh::checkCollision(Collider *col)
{
    return col->checkCollision(this);
}

bool ColliderMesh::checkCollision(ColliderMesh *col)
{

}

bool ColliderMesh::checkCollision(ColliderSphere *col)
{

}