#include "TriggerCylinder.h"

#include "Collider.h"
#include "ColliderSphere.h"
#include "ColliderMesh.h"

TriggerCylinder::TriggerCylinder(float radius, float length) :
    Trigger(vec3(-radius, 0, -radius), vec3(radius, length, radius)), radius(radius), length(length)
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