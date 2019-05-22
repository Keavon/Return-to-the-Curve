#include "Octree.h"

#include "PhysicsObject.h"
#include "../Program.h"
#include "../MatrixStack.h"

#include <algorithm>
#include <iostream>
#include <glm/glm.hpp>
#include <unordered_set>
#include <stack>
#include <memory>

using namespace glm;
using namespace std;

// https://www.gamedev.net/articles/programming/general-and-gameplay-programming/introduction-to-octrees-r3529/

OctNode::OctNode(vec3 min, vec3 max, OctNode *parent) :
    imin(min), imax(max), dimensions(max - min), center(min + dimensions / 2.0f),
    parent(parent), numObjects(0)
{
    for (int i = 0; i < 8; i++)
    {
        octant[i] = nullptr;
    }
}

OctNode::OctNode(vec3 min, vec3 max) : OctNode(min, max, NULL)
{
}

bool boxContainsSphere(vec3 min, vec3 max, vec3 position, float radius)
{
    vec3 sphereMin = position - vec3(radius);
    vec3 sphereMax = position + vec3(radius);
    return sphereMin.x > min.x && sphereMin.y > min.y && sphereMin.z > min.z &&
        sphereMax.x < max.x && sphereMax.y < max.y && sphereMax.z < max.z;
}

void OctNode::insert(shared_ptr<PhysicsObject> object)
{
    numObjects++;

    // base case: no other objects in this node
    if (numObjects == 1)
    {
        elements.insert(object);
        return;
    }

    if (octant[0] == nullptr)
    {
        octant[0] = make_shared<OctNode>(imin, center, this);
        octant[1] = make_shared<OctNode>(vec3(center.x, imin.y, imin.z), vec3(imax.x, center.y, center.z), this);
        octant[2] = make_shared<OctNode>(vec3(center.x, imin.y, center.z), vec3(imax.x, center.y, imax.z), this);
        octant[3] = make_shared<OctNode>(vec3(imin.x, imin.y, center.z), vec3(center.x, center.y, imax.z), this);
        octant[4] = make_shared<OctNode>(vec3(imin.x, center.y, imin.z), vec3(center.x, imax.y, center.z), this);
        octant[5] = make_shared<OctNode>(vec3(center.x, center.y, imin.z), vec3(imax.x, imax.y, center.z), this);
        octant[6] = make_shared<OctNode>(center, imax, this);
        octant[7] = make_shared<OctNode>(vec3(imin.x, center.y, center.z), vec3(center.x, imax.y, imax.z), this);

        // Move existing objects to child nodes if they fit
        vector<shared_ptr<PhysicsObject>> moved;

        for (auto element : elements)
        {
            for (int i = 0; i < 8; i++)
            {
                if (boxContainsSphere(octant[i]->imin, octant[i]->imax, element->position, element->getRadius()))
                {
                    moved.push_back(element);
                    octant[i]->insert(element);
                    break;
                }
            }
        }

        for (auto element : moved)
        {
            elements.erase(element);
        }
    }


    // Recursive insert
    for (int i = 0; i < 8; i++)
    {
        if (boxContainsSphere(octant[i]->imin, octant[i]->imax, object->position, object->getRadius()))
        {
            octant[i]->insert(object);
            return;
        }
    }

    elements.insert(object);
}

void OctNode::update()
{
    // Recursive update
    if (octant[0] != nullptr)
    {
        for (int i = 0; i < 8; i++)
        {
            octant[i]->update();
        }
    }

    // Remove objects which have moved
    vector<shared_ptr<PhysicsObject>> moved;

    for (auto object : elements)
    {
        if (object->velocity != vec3(0))
        {
            moved.push_back(object);
        }
    }

    for (auto object : moved)
    {
        elements.erase(object);
    }

    // Traverse up the tree until an octant large enough for the object is found
    for (auto object : moved)
    {
        OctNode *current = this;

        while (current->parent != NULL)
        {
            current->numObjects--;
            if (boxContainsSphere(current->imin, current->imax, object->position, object->getRadius()))
            {
                break;
            }
            else
            {
                current = current->parent;
            }
        }

        current->insert(object);
    }

    // Move leaf node up if possible
    if (parent != NULL && numObjects == 1 && parent->numObjects == 1)
    {
        auto object = *(elements.begin());
        elements.erase(object);
        parent->elements.insert(object);
        numObjects--;
    }

    // Remove empty nodes
    if (numObjects == elements.size())
    {
        for (int i = 0; i < 8; i++)
        {
            octant[i] = nullptr;
        }
    }
}


Octree::Octree(vec3 min, vec3 max) : imin(min), imax(max), debug(false)
{
}

vector<shared_ptr<PhysicsObject>> Octree::query(shared_ptr<PhysicsObject> object)
{
    vector<shared_ptr<PhysicsObject>> hits;

    stack<shared_ptr<OctNode>> nodes;

    if (root == nullptr)
    {
        return hits;
    }

    nodes.push(root);

    bool end = false;
    while (!end)
    {
        end = true;
        for (int i = 0; i < 8; i++)
        {
            if (nodes.top()->octant[i] != nullptr &&
                boxContainsSphere(nodes.top()->octant[i]->imin, nodes.top()->octant[i]->imax, object->position, object->getRadius()))
            {
                nodes.push(nodes.top()->octant[i]);
                end = false;
                break;
            }
        }
    }

    while (!nodes.empty())
    {
        for (auto otherObject : nodes.top()->elements)
        {
            if (otherObject != object)
            {
                hits.push_back(otherObject);
            }
        }
        nodes.pop();
    }

    return hits;
}

void Octree::init(shared_ptr<Shape> billboard, shared_ptr<Shape> cube)
{
    this->billboard = billboard;
    this->cube = cube;
}

void Octree::drawDebugBoundingSpheres(shared_ptr<Program> prog)
{
    auto M = make_shared<MatrixStack>();

    for (auto object : objects)
    {
        M->pushMatrix();
            M->translate(object->position);
            M->scale(object->getRadius());
            glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
            glUniform1f(prog->getUniform("radius"), object->getRadius());
            billboard->draw(prog);
        M->popMatrix();
    }
}

void Octree::drawDebugOctants(shared_ptr<Program> prog)
{
    auto M = make_shared<MatrixStack>();

    glDisable(GL_CULL_FACE);

    stack<shared_ptr<OctNode>> stack;
    if (root != nullptr)
    {
        stack.push(root);
    }

    while (!stack.empty())
    {
        shared_ptr<OctNode> node = stack.top();
        stack.pop();
        for (int i = 0; i < 8; i++)
        {
            if (node->octant[i] != nullptr)
            {
                stack.push(node->octant[i]);
            }
        }

        M->pushMatrix();
            M->translate(node->center);
            M->scale(node->dimensions / 2.0f);
			glUniform1f(prog->getUniform("edge"), dot(node->dimensions, vec3(0.5)) / 3.0f);
            glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
            cube->draw(prog);
        M->popMatrix();
    }

    glEnable(GL_CULL_FACE);
}

void Octree::clear()
{
    root = nullptr;
}

void Octree::update()
{
    if (root != nullptr)
    {
        root->update();
    }
}

void Octree::insert(shared_ptr<PhysicsObject> object)
{
    if (root == nullptr)
    {
        root = make_shared<OctNode>(imin, imax);
    }
    root->insert(object);
    objects.insert(object);
}

void Octree::insert(vector<shared_ptr<PhysicsObject>> objects)
{
    for (auto object : objects)
    {
        insert(object);
    }
}