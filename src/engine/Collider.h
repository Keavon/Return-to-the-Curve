#pragma once

#include "BoundingBox.h"

#include <glm/glm.hpp>
#include <vector>

// https://eli.thegreenplace.net/2016/a-polyglots-guide-to-multiple-dispatch/
// https://gamedevelopment.tutsplus.com/tutorials/how-to-create-a-custom-2d-physics-engine-the-basics-and-impulse-resolution--gamedev-6331

class ColliderMesh;
class ColliderSphere;
class TriggerSphere;
class TriggerCylinder;
class PhysicsObject;

struct Collision {
    PhysicsObject *other;
    float penetration;
    glm::vec3 normal;
};

class Collider
{
public:
    Collider(glm::vec3 min, glm::vec3 max);

    virtual void checkCollision(PhysicsObject *owner, PhysicsObject *obj, Collider *col) = 0;
    virtual void checkCollision(PhysicsObject *owner, PhysicsObject *obj, ColliderMesh *col) {};
    virtual void checkCollision(PhysicsObject *owner, PhysicsObject *obj, ColliderSphere *col) {};
    virtual void checkCollision(PhysicsObject *owner, PhysicsObject *obj, TriggerSphere *col) {};
    virtual void checkCollision(PhysicsObject *owner, PhysicsObject *obj, TriggerCylinder *col) {};

    BoundingBox bbox;

    std::vector<Collision> pendingCollisions;
};

void checkSphereMesh(PhysicsObject *sphere, ColliderSphere *sphereCol, PhysicsObject *mesh, ColliderMesh *meshCol);
void checkSphereSphere(PhysicsObject *sphere1, ColliderSphere *sphereCol1, PhysicsObject *sphere2, ColliderSphere *sphereCol2);
void checkColSphereTriggerSphere(PhysicsObject *cSphere, ColliderSphere *cSphereCol, PhysicsObject *tSphere, TriggerSphere *tSphereTrig);
void checkColSphereTriggerCylinder(PhysicsObject *sphere, ColliderSphere *sphereCol, PhysicsObject *cylinder, TriggerCylinder *cylinderTrig);
