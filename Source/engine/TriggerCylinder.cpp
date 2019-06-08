#include "TriggerCylinder.h"

TriggerCylinder::TriggerCylinder(float radius, float length) :
    Collider(vec3(-radius, -length/2, -radius), vec3(radius, length/2, radius)), radius(radius)
{
}

void TriggerCylinder::checkCollision(PhysicsObject *owner, PhysicsObject *obj, Collider *col)
{
    col->checkCollision(obj, owner, this);
}

void TriggerCylinder::checkCollision(PhysicsObject *owner, PhysicsObject *obj, ColliderSphere *col)
{
    checkColSphereTriggerCylinder(obj, col, owner, this);
}

float TriggerCylinder::getRadius(vec3 scale)
{
    return glm::length(vec3(radius, length, radius) * scale);
}