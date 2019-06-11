#include "TriggerSphere.h"

#include "Collider.h"
#include "ColliderSphere.h"
#include "PhysicsObject.h"

TriggerSphere::TriggerSphere(float radius) :
    Trigger(vec3(-radius), vec3(radius)), radius(radius)
{
}

void TriggerSphere::checkCollision(PhysicsObject *owner, PhysicsObject *obj, Collider *col)
{
    col->checkCollision(obj, owner, this);
}

void TriggerSphere::checkCollision(PhysicsObject *owner, PhysicsObject *obj, ColliderSphere *col)
{
    checkColSphereTriggerSphere(obj, col, owner, this);
}

float TriggerSphere::getRadius(vec3 scale)
{
    return bbox.radius * scale.x;
}
