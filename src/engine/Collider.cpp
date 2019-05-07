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

#include <unistd.h>

using namespace glm;
using namespace std;

Collider::Collider(vec3 min, vec3 max) :
    bbox(min, max)
{
}

void checkSphereSphere(PhysicsObject *sphere1, ColliderSphere *sphereCol1, PhysicsObject *sphere2, ColliderSphere *sphereCol2)
{

}

void checkSphereMesh(PhysicsObject *sphere, ColliderSphere *sphereCol, PhysicsObject *mesh, ColliderMesh *meshCol)
{
    mat4 M = glm::translate(glm::mat4(1.f), mesh->position) * glm::mat4_cast(mesh->orientation) * glm::scale(glm::mat4(1.f), mesh->scale);

    // Check bounding spheres
    if (distance2(sphere->position, mesh->position) <= pow(sphereCol->bbox.getRadius(sphere->scale) + meshCol->bbox.getRadius(mesh->scale), 2))
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
            bool rayDidIntersect = intersectRayPlane(sphere->position, dir, v[0], normal, d);

            if (rayDidIntersect && fabs(d) < sphereCol->radius)
            {
                rayDidIntersect = intersectRayTriangle(sphere->position, dir, v[0], v[1], v[2], bary, d);

                if (rayDidIntersect)
                {
                    Collision collision;
                    collision.other = mesh;
                    collision.normal = dir;
                    collision.penetration = sphereCol->radius - d;
                    sphereCol->pendingCollisions.push_back(collision);
                }
                else
                {
                    bool hitEdge = false;
                    for (int i = 0; i < 3; i++)
                    {
                        // check edges
                        int e0 = i;
                        int e1 = (i + 1) % 3;
                        vec3 closestPoint = v[e0] + proj(sphere->position - v[e0], normalize(v[e1] - v[e0]));
                        d = distance(sphere->position, closestPoint);

                        if (d < sphereCol->radius &&
                            dot(v[e1] - v[e0], closestPoint - v[e0]) > 0 && dot(v[e0] - v[e1], closestPoint - v[e1]) > 0)
                        {
                            Collision collision;
                            collision.other = mesh;
                            collision.normal = normalize(closestPoint - sphere->position);
                            collision.penetration = sphereCol->radius - d;
                            sphereCol->pendingCollisions.push_back(collision);
                            hitEdge = true;
                            break;
                        }
                    }

                    if (!hitEdge)
                    {
                        //check vertices
                        for (int i = 0; i < 3; i++)
                        {
                            d = distance(sphere->position, v[i]);
                            if (d < sphereCol->radius)
                            {
                                Collision collision;
                                collision.other = mesh;
                                collision.normal = normalize(v[0] - sphere->position);
                                collision.penetration = sphereCol->radius - d;
                                sphereCol->pendingCollisions.push_back(collision);
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
}

void checkColSphereTriggerSphere(PhysicsObject *cSphere, ColliderSphere *cSphereCol, PhysicsObject *tSphere, TriggerSphere *tSphereTrig)
{
    if (distance2(cSphere->position, tSphere->position) < powf(cSphereCol->radius + tSphereTrig->radius, 2))
    {
        Collision collision;
        collision.other = tSphere;
        tSphereTrig->pendingCollisions.push_back(collision);
    }
}

void checkColSphereTriggerCylinder(PhysicsObject *sphere, ColliderSphere *sphereCol, PhysicsObject *cylinder, TriggerCylinder *cylinderTrig)
{
    
}
