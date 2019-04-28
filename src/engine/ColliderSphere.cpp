#include "ColliderSphere.h"
#include "ColliderMesh.h"
#include "Collider.h"

#include "BoundingBox.h"
#include <glm/glm.hpp>

using namespace glm;

ColliderSphere::ColliderSphere(vec3 *position, quat *orientation, float radius) :
    Collider(position, orientation, vec3(-radius), vec3(radius)), radius(radius)
{
}

bool ColliderSphere::checkCollision(Collider *col)
{
    return col->checkCollision(this);
}

bool ColliderSphere::checkCollision(ColliderMesh *col)
{

}

bool ColliderSphere::checkCollision(ColliderSphere *col)
{

}