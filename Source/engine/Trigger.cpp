#include "Trigger.h"
#include "Collider.h"
#include "ColliderSphere.h"
#include "TriggerSphere.h"
#include "TriggerCylinder.h"

#include <algorithm>

// Also invoke callbacks
void Trigger::clearCollisions(PhysicsObject *owner)
{
    Collider::clearCollisions(owner);

    for (auto object : currentObjects)
    {
        if (prevObjects.find(object) == prevObjects.end())
        {
            owner->triggerEnter(object);
            owner->triggerStay(object);
        }
        else
        {
            owner->triggerStay(object);
        }
    }
    for (auto object : prevObjects)
    {
        if (currentObjects.find(object) == currentObjects.end())
        {
            owner->triggerExit(object);
        }
    }
    
    prevObjects = currentObjects;
    currentObjects.clear();
}


void checkColSphereTriggerSphere(PhysicsObject *cSphere, ColliderSphere *cSphereCol, PhysicsObject *tSphere, TriggerSphere *tSphereTrig)
{
    if (distance2(cSphere->position, tSphere->position) < powf(cSphere->getRadius() + tSphere->getRadius(), 2))
    {
        tSphereTrig->currentObjects.insert(cSphere);
    }
}

void checkColSphereTriggerCylinder(PhysicsObject *sphere, ColliderSphere *sphereCol, PhysicsObject *cylinder, TriggerCylinder *cylinderTrig)
{
    if (distance2(sphere->getCenterPos(), cylinder->getCenterPos()) <= pow(sphere->getRadius() + cylinder->getRadius(), 2))
    {
        mat4 M = mat4_cast(inverse(cylinder->orientation)) * translate(mat4(1), -cylinder->position);
        vec3 transformedSpherePos = vec3(M * vec4(sphere->position, 1));
        float d2 = length2(vec2(transformedSpherePos.x, transformedSpherePos.z));
        if (d2 < pow(cylinderTrig->radius + sphere->getRadius(), 2) &&
            transformedSpherePos.y > -sphere->getRadius() &&
            transformedSpherePos.y < cylinderTrig->length + sphere->getRadius())
        {
            cylinderTrig->currentObjects.insert(sphere);
        }
    }
}
