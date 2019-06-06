#include "ColliderSphere.h"
#include "ColliderMesh.h"
#include "TriggerSphere.h"
#include "TriggerCylinder.h"
#include "Collider.h"
#include "PhysicsObject.h"

#include "BoundingBox.h"
#include <glm/glm.hpp>

using namespace glm;

ColliderSphere::ColliderSphere(float radius) :
    Collider(radius), radius(radius)
{
}

void ColliderSphere::checkCollision(PhysicsObject *owner, PhysicsObject *obj, Collider *col)
{
    col->checkCollision(obj, owner, this);
}

void ColliderSphere::checkCollision(PhysicsObject *owner, PhysicsObject *obj, ColliderMesh *col)
{
    checkSphereMesh(owner, this, obj, col);
}

void ColliderSphere::checkCollision(PhysicsObject *owner, PhysicsObject *obj, ColliderSphere *col)
{
    checkSphereSphere(owner, this, obj, col);
}

void ColliderSphere::checkCollision(PhysicsObject *owner, PhysicsObject *obj, TriggerSphere *col)
{
    checkColSphereTriggerSphere(owner, this, obj, col);
}

void ColliderSphere::checkCollision(PhysicsObject *owner, PhysicsObject *obj, TriggerCylinder *col)
{
    checkColSphereTriggerCylinder(owner, this, obj, col);
}

float ColliderSphere::getRadius(glm::vec3 scale)
{
    return bbox.radius * scale.x;
}