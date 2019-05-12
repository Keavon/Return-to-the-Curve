#pragma once

#include <memory>
#include <vector>
#include <unordered_set>
#include <glm/glm.hpp>
#include "PhysicsObject.h"
#include "../Program.h"
#include "../Shape.h"
#include "../MatrixStack.h"

#define MAX_DEPTH
#define MAX_ELEMENTS

class OctNode
{
public:
    OctNode(glm::vec3 min, glm::vec3 max);
    void build();
    std::shared_ptr<OctNode> octant[8];
    std::unordered_set<std::shared_ptr<PhysicsObject>> elements;
    glm::vec3 min;
    glm::vec3 max;
    glm::vec3 dimensions;
    glm::vec3 center;
};

class Octree
{
public:
    Octree(glm::vec3 min, glm::vec3 max);
    void init(std::shared_ptr<Shape> billboard, std::shared_ptr<Shape> cube);
    std::vector<std::shared_ptr<PhysicsObject>> query(std::shared_ptr<PhysicsObject> object);
    void queue(std::shared_ptr<PhysicsObject> object);
    void queue(std::vector<std::shared_ptr<PhysicsObject>> objects);
    void drawDebugBoundingSpheres(std::shared_ptr<Program> prog);
    void drawDebugOctants(std::shared_ptr<Program> prog);
    void clear();
    void build();
    std::shared_ptr<OctNode> root;

    bool debug;

private:
    std::shared_ptr<Shape> billboard;
    std::shared_ptr<Shape> cube;
    std::vector<std::shared_ptr<PhysicsObject>> objects;
    glm::vec3 min, max;
};