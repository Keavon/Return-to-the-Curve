#include "Octree.h"

#include "PhysicsObject.h"
#include "../Program.h"
#include "../MatrixStack.h"

#include <iostream>
#include <glm/glm.hpp>
#include <unordered_set>
#include <stack>
#include <memory>

using namespace glm;
using namespace std;

// https://www.gamedev.net/articles/programming/general-and-gameplay-programming/introduction-to-octrees-r3529/

OctNode::OctNode(vec3 min, vec3 max) :
    imin(min), imax(max), dimensions(max - min), center(min + dimensions / 2.0f)
{
    for (int i = 0; i < 8; i++)
    {
        octant[i] = nullptr;
    }
}

bool boxContainsSphere(vec3 min, vec3 max, vec3 position, float radius)
{
    vec3 sphereMin = position - vec3(radius);
    vec3 sphereMax = position + vec3(radius);
    return sphereMin.x > min.x && sphereMin.y > min.y && sphereMin.z > min.z &&
        sphereMax.x < max.x && sphereMax.y < max.y && sphereMax.z < max.z;
}

void OctNode::build()
{
    if (elements.size() <= 1)
    {
        return;
    }

    octant[0] = make_shared<OctNode>(imin, center);
    octant[1] = make_shared<OctNode>(vec3(center.x, imin.y, imin.z), vec3(imax.x, center.y, center.z));
    octant[2] = make_shared<OctNode>(vec3(center.x, imin.y, center.z), vec3(imax.x, center.y, imax.z));
    octant[3] = make_shared<OctNode>(vec3(imin.x, imin.y, center.z), vec3(center.x, center.y, imax.z));
    octant[4] = make_shared<OctNode>(vec3(imin.x, center.y, imin.z), vec3(center.x, imax.y, center.z));
    octant[5] = make_shared<OctNode>(vec3(center.x, center.y, imin.z), vec3(imax.x, imax.y, center.z));
    octant[6] = make_shared<OctNode>(center, imax);
    octant[7] = make_shared<OctNode>(vec3(imin.x, center.y, center.z), vec3(center.x, imax.y, imax.z));

    vector<shared_ptr<PhysicsObject>> delist;

    for (auto element : elements)
    {
        for (int i = 0; i < 8; i++)
        {
            if (boxContainsSphere(octant[i]->imin, octant[i]->imax, element->position, element->getRadius()))
            {
                octant[i]->elements.insert(element);
                delist.push_back(element);
                break;
            }
        }
    }

    for (auto element : delist)
    {
        elements.erase(element);
    }

    for (int i = 0; i < 8; i++)
    {
        if (!octant[i]->elements.empty())
        {
            octant[i]->build();
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

void Octree::queue(shared_ptr<PhysicsObject> object)
{
    objects.push_back(object);
}

void Octree::queue(vector<shared_ptr<PhysicsObject>> objects)
{
    this->objects.insert(this->objects.end(), objects.begin(), objects.end());
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

void Octree::build()
{
    root = make_shared<OctNode>(imin, imax);
    root->elements.insert(objects.begin(), objects.end());
    root->build();
}