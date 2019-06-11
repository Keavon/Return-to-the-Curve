#pragma once

#include "Collider.h"
#include <unordered_set>
#include <memory>

class TriggerCylinder;
class TriggerSphere;
class PhysicsObject;
class ColliderSphere;

class Trigger : public Collider
{
public:
    using Collider::Collider;

    virtual void clearCollisions(PhysicsObject *owner);

    std::unordered_set<PhysicsObject *> prevObjects;
    std::unordered_set<PhysicsObject *> currentObjects;
};

void checkColSphereTriggerSphere(PhysicsObject *cSphere, ColliderSphere *cSphereCol, PhysicsObject *tSphere, TriggerSphere *tSphereTrig);
void checkColSphereTriggerCylinder(PhysicsObject *sphere, ColliderSphere *sphereCol, PhysicsObject *cylinder, TriggerCylinder *cylinderTrig);