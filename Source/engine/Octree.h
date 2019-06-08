#pragma once

#include <algorithm>
#include <iostream>
#include <glm/glm.hpp>
#include <unordered_set>
#include <stack>
#include <memory>
#include <vector>

#include "PhysicsObject.h"
#include "Frustum.h"
#include "../Program.h"
#include "../MatrixStack.h"
#include "../Shape.h"

#define MAX_DEPTH
#define MAX_ELEMENTS

using namespace glm;
using namespace std;

class OctNode
{
public:
    OctNode(vec3 min, vec3 max);
    OctNode(vec3 min, vec3 max, OctNode *parent);
    void update();
    void insert(shared_ptr<PhysicsObject> object);

    shared_ptr<OctNode> octant[8];
    unordered_set<shared_ptr<PhysicsObject>> elements;
    vec3 imin;
    vec3 imax;
    vec3 dimensions;
    vec3 center;
    int numObjects;
    OctNode *parent;
};

class Octree
{
public:
    Octree();
    Octree(vec3 min, vec3 max);
    void init(shared_ptr<Shape> billboard, shared_ptr<Shape> cube);
    vector<shared_ptr<PhysicsObject>> query(shared_ptr<PhysicsObject> object);
    vector<shared_ptr<PhysicsObject>> query(Frustum &viewFrustum);
    void markInView(Frustum &viewFrustum);
    void drawDebugBoundingSpheres(shared_ptr<Program> prog);
    void drawDebugOctants(shared_ptr<Program> prog);
    void clear();
    void update();
    void insert(shared_ptr<PhysicsObject> object);
    void insert(vector<shared_ptr<PhysicsObject>> object);
    void fitToObjects();

    shared_ptr<OctNode> root;
    bool debug;

private:
    shared_ptr<Shape> billboard;
    shared_ptr<Shape> cube;
    unordered_set<shared_ptr<PhysicsObject>> objects;
    vec3 imin, imax;
};