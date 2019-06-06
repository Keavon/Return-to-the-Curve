#include "TriggerCylinder.h"
#include "ColliderMesh.h"
#include "Collider.h"
#include "PhysicsObject.h"

#include "BoundingBox.h"
#include <glm/glm.hpp>

using namespace glm;

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

float TriggerCylinder::getRadius(glm::vec3 scale)
{
    return glm::length(vec3(radius, length, radius) * scale);
}