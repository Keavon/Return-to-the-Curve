#include "Collider.h"

#include "ColliderSphere.h"
#include "ColliderMesh.h"
#include "PhysicsObject.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/intersect.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/projection.hpp>
#include <glm/glm.hpp>
#include <vector>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <unordered_set>

//#include <unistd.h>

using namespace glm;
using namespace std;

Collider::Collider(vec3 min, vec3 max) :
    bbox(min, max)
{
}

Collider::Collider(float radius) :
    bbox(radius)
{
}

void checkSphereSphere(PhysicsObject *sphere1, ColliderSphere *sphereCol1, PhysicsObject *sphere2, ColliderSphere *sphereCol2)
{
    float d = distance(sphere1->position, sphere2->position);
    if (d < sphere1->getRadius() + sphere2->getRadius())
    {
        Collision collision1;
        collision1.other = sphere2;
        collision1.normal = -normalize(sphere1->position - sphere2->position);
        collision1.penetration = sphere1->getRadius() + sphere2->getRadius() - d;
        sphereCol1->pendingCollisions.push_back(collision1);

        Collision collision2;
        collision2.other = sphere1;
        collision2.normal = collision1.normal;
        collision2.penetration = collision1.penetration;
        sphereCol2->pendingCollisions.push_back(collision2);
    }
}



void checkSphereMesh(PhysicsObject *sphere, ColliderSphere *sphereCol, PhysicsObject *mesh, ColliderMesh *meshCol)
{
    mat4 M = glm::translate(glm::mat4(1.f), mesh->position) * glm::mat4_cast(mesh->orientation) * glm::scale(glm::mat4(1.f), mesh->scale);

    unordered_set<Edge, EdgeHash> edgeSet;
    unordered_set<vec3> vertSet;

    // Check bounding spheres
    if (distance2(sphere->position, mesh->position) <= pow(sphere->getRadius() + mesh->getRadius(), 2))
    {
        // Check faces
        for (int i = 0; i < meshCol->mesh->getNumFaces(); i++)
        {
            vector<vec3> v = meshCol->mesh->getFace(i, M);

            // Check if sphere is touching triangle
            vec3 normal = normalize(cross(v[1] - v[0], v[2] - v[0]));
            vec3 dir = -normal;
            vec2 bary;
            float d;
            bool rayDidIntersect = intersectRayTriangle(sphere->position, dir, v[0], v[1], v[2], bary, d);

            if (rayDidIntersect && fabs(d) < sphere->getRadius())
            {
                Collision collision;
                collision.other = mesh;
                collision.normal = dir;
                collision.penetration = sphere->getRadius() - d;
                sphereCol->pendingCollisions.push_back(collision);

                // add edges of triangle to set of edges we shouldn't check
                edgeSet.insert(Edge(v[0], v[1]));
                edgeSet.insert(Edge(v[1], v[2]));
                edgeSet.insert(Edge(v[2], v[0]));
            }
        }

        // Check edges
        for (int i = 0; i < meshCol->mesh->getNumEdges(); i++)
        {
            vector<vec3> v = meshCol->mesh->getEdge(i, M);

            if (edgeSet.find(Edge(v[0], v[1])) != edgeSet.end())
            {
                // skip edge if it's in the set
                continue;
            }

            vec3 closestPoint = v[0] + proj(sphere->position - v[0], normalize(v[1] - v[0]));
            float d = distance(sphere->position, closestPoint);

            if (d < sphere->getRadius() &&
                dot(v[1] - v[0], closestPoint - v[0]) > 0 && dot(v[0] - v[1], closestPoint - v[1]) > 0)
            {
                Collision collision;
                collision.other = mesh;
                collision.normal = normalize(closestPoint - sphere->position);
                collision.penetration = sphere->getRadius() - d;
                sphereCol->pendingCollisions.push_back(collision);

                // add vertices of edge to set of vertices we souldn't check 
                vertSet.insert(v[0]);
                vertSet.insert(v[1]);
            }
        }

        // check vertices
        for (int i = 0; i < meshCol->mesh->getNumVertices(); i++)
        {
            vec3 v = meshCol->mesh->getVertex(i, M);

            if (vertSet.find(v) != vertSet.end())
            {
                // skip vertex if it's in the set
                continue;
            }

            float d = distance(sphere->position, v);
            if (d < sphere->getRadius())
            {
                Collision collision;
                collision.other = mesh;
                collision.normal = normalize(v - sphere->position);
                collision.penetration = sphere->getRadius() - d;
                sphereCol->pendingCollisions.push_back(collision);
            }
        }
    }
}

void checkColSphereTriggerSphere(PhysicsObject *cSphere, ColliderSphere *cSphereCol, PhysicsObject *tSphere, TriggerSphere *tSphereTrig)
{
    if (distance2(cSphere->position, tSphere->position) < powf(cSphere->getRadius() + tSphere->getRadius(), 2))
    {
        Collision collision;
        collision.other = cSphere;
        tSphereTrig->pendingCollisions.push_back(collision);
    }
}

void checkColSphereTriggerCylinder(PhysicsObject *sphere, ColliderSphere *sphereCol, PhysicsObject *cylinder, TriggerCylinder *cylinderTrig)
{
    
}
