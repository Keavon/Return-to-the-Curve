#pragma once

#include "BoundingBox.h"

#include <glm/glm.hpp>

// https://eli.thegreenplace.net/2016/a-polyglots-guide-to-multiple-dispatch/

struct Collision {
    bool hit;
    float t;
    glm::vec3 point;
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 relativeVelocity;
};


class ColliderMesh;
class ColliderSphere;
class PhysicsObject;

class Collider
{
public:
    Collider(glm::vec3 min, glm::vec3 max);

    virtual void checkCollision(PhysicsObject *owner, PhysicsObject *obj, Collider *col) = 0;
    virtual void checkCollision(PhysicsObject *owner, PhysicsObject *obj, ColliderMesh *col) {};
    virtual void checkCollision(PhysicsObject *owner, PhysicsObject *obj, ColliderSphere *col) {};

    BoundingBox bbox;

    Collision pendingCollision;
};

void checkSphereMesh(PhysicsObject *sphere, ColliderSphere *sphereCol, PhysicsObject *mesh, ColliderMesh *meshCol);
void checkSphereSphere(PhysicsObject *sphere1, ColliderSphere *sphereCol1, PhysicsObject *sphere2, ColliderSphere *sphereCol2);

void checkDynamicSphereKinematicMesh(PhysicsObject *dSphere, ColliderSphere *dSphereCol, PhysicsObject *kMesh, ColliderMesh *kMeshCol);
void checkDynamicSphereKinematicSphere(PhysicsObject *dSphere, ColliderSphere *dSphereCol, PhysicsObject *kSphere, ColliderSphere *kSphereCol);