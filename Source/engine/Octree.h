#pragma once

#include <memory>
#include <vector>
#include <unordered_set>
#include <glm/glm.hpp>
#include "PhysicsObject.h"
#include "../Program.h"
#include "../Shape.h"
#include "../MatrixStack.h"
#include "Frustum.h"

#define MAX_DEPTH
#define MAX_ELEMENTS

class OctNode
{
public:
    OctNode(glm::vec3 min, glm::vec3 max);
    OctNode(glm::vec3 min, glm::vec3 max, OctNode *parent);
    void update();
    void insert(std::shared_ptr<PhysicsObject> object);

    std::shared_ptr<OctNode> octant[8];
    std::unordered_set<std::shared_ptr<PhysicsObject>> elements;
    glm::vec3 imin;
    glm::vec3 imax;
    glm::vec3 dimensions;
    glm::vec3 center;
    int numObjects;
    OctNode *parent;
};

class Octree
{
public:
    Octree();
    Octree(glm::vec3 min, glm::vec3 max);
    void init(std::shared_ptr<Shape> billboard, std::shared_ptr<Shape> cube);
    std::vector<std::shared_ptr<PhysicsObject>> query(std::shared_ptr<PhysicsObject> object);
    std::vector<std::shared_ptr<PhysicsObject>> query(Frustum &viewFrustum);
    void markInView(Frustum &viewFrustum);
    void drawDebugBoundingSpheres(std::shared_ptr<Program> prog);
    void drawDebugOctants(std::shared_ptr<Program> prog);
    void clear();
    void update();
    void insert(std::shared_ptr<PhysicsObject> object);
    void insert(std::vector<std::shared_ptr<PhysicsObject>> object);
    void fitToObjects();

    std::shared_ptr<OctNode> root;
    bool debug;

private:
    std::shared_ptr<Shape> billboard;
    std::shared_ptr<Shape> cube;
    std::unordered_set<std::shared_ptr<PhysicsObject>> objects;
    glm::vec3 imin, imax;
};