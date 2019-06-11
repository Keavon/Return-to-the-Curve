#pragma once

#include "BoundingBox.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#include <glm/gtx/intersect.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/projection.hpp>
#include <glm/glm.hpp>

#include <vector>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <unordered_set>

// https://eli.thegreenplace.net/2016/a-polyglots-guide-to-multiple-dispatch/
// https://gamedevelopment.tutsplus.com/tutorials/how-to-create-a-custom-2d-physics-engine-the-basics-and-impulse-resolution--gamedev-6331

using namespace glm;
using namespace std;

class ColliderMesh;
class ColliderSphere;
class TriggerSphere;
class TriggerCylinder;
class Trigger;
class PhysicsObject;

enum ColGeom {FACE, EDGE, VERT, SPHERE};

struct Collision {
    PhysicsObject *other;
    float penetration;
    vec3 normal;
    ColGeom geom;

    vec3 v[3];
    vec3 pos;
};

class Collider
{
public:
    Collider(vec3 min, vec3 max);
    Collider(float radius);

    virtual void checkCollision(PhysicsObject *owner, PhysicsObject *obj, Collider *col) = 0;
    virtual void checkCollision(PhysicsObject *owner, PhysicsObject *obj, ColliderMesh *col) {};
    virtual void checkCollision(PhysicsObject *owner, PhysicsObject *obj, ColliderSphere *col) {};
    virtual void checkCollision(PhysicsObject *owner, PhysicsObject *obj, TriggerSphere *col) {};
    virtual void checkCollision(PhysicsObject *owner, PhysicsObject *obj, TriggerCylinder *col) {};

    virtual void clearCollisions(PhysicsObject *owner);
    virtual float getRadius(vec3 scale) = 0;

    BoundingBox bbox;

    vector<Collision> pendingCollisions;
};

void checkSphereMesh(PhysicsObject *sphere, ColliderSphere *sphereCol, PhysicsObject *mesh, ColliderMesh *meshCol);
void checkSphereSphere(PhysicsObject *sphere1, ColliderSphere *sphereCol1, PhysicsObject *sphere2, ColliderSphere *sphereCol2);


// Used for inserting pairs of vertices into a hash set
struct Edge
{
    vec3 v0, v1;

    Edge(vec3 v0, vec3 v1) : v0(v0), v1(v1)
    {
    }

    bool operator==(const Edge &e) const
    {
        return (v0 == e.v0 && v1 == e.v1) ||
            (v0 == e.v1 && v1 == e.v0);
    }
};

class EdgeHash
{
public:
    size_t operator()(const Edge &e) const
    {
        return hash<vec3>()(e.v0) ^ hash<vec3>()(e.v1);
    }
};