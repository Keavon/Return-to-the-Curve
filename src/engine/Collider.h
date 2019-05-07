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
};


class ColliderMesh;
class ColliderSphere;
class PhysicsObject;

class Collider
{
public:
    Collider(PhysicsObject *parent, glm::vec3 min, glm::vec3 max);

    virtual bool checkCollision(Collider *col) = 0;
    virtual bool checkCollision(ColliderMesh *col) {};
    virtual bool checkCollision(ColliderSphere *col) {};

    BoundingBox bbox;
    PhysicsObject *parent;

    Collision pendingCollision;
};

bool checkSphereMesh(ColliderSphere *sphere, ColliderMesh *mesh);