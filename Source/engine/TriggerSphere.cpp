#include "TriggerSphere.h"
#include "ColliderMesh.h"
#include "Collider.h"
#include "PhysicsObject.h"

#include "BoundingBox.h"
#include <glm/glm.hpp>

using namespace glm;

TriggerSphere::TriggerSphere(float radius) :
    Collider(vec3(-radius), vec3(radius)), radius(radius)
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

float TriggerSphere::getRadius(glm::vec3 scale)
{
    return bbox.radius * scale.x;
}
