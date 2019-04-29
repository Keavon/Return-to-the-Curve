#include "ColliderSphere.h"
#include "ColliderMesh.h"
#include "Collider.h"
#include "PhysicsObject.h"

#include "BoundingBox.h"
#include <glm/glm.hpp>

using namespace glm;

ColliderSphere::ColliderSphere(PhysicsObject *parent, float radius) :
    Collider(parent, vec3(-radius), vec3(radius)), radius(radius)
{
}

bool ColliderSphere::checkCollision(Collider *col)
{
    return col->checkCollision(this);
}

bool ColliderSphere::checkCollision(ColliderMesh *col)
{
    return checkSphereMesh(this, col);
}

bool ColliderSphere::checkCollision(ColliderSphere *col)
{
    // not handled
}