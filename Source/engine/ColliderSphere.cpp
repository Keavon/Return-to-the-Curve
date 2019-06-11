#include "ColliderSphere.h"

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

float ColliderSphere::getRadius(vec3 scale)
{
    return bbox.radius * scale.x;
}