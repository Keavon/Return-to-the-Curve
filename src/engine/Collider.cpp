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

void checkSphereMesh(PhysicsObject *sphere, ColliderSphere *sphereCol, PhysicsObject *mesh, ColliderMesh *meshCol)
{
    if (sphere->mass == 0 && mesh->mass == 0)
    {
        // not implemented
    }
    else if (sphere->mass == 0)
    {
        // not implemented
    }
    else if (mesh->mass == 0)
    {
        checkDynamicSphereKinematicMesh(sphere, sphereCol, mesh, meshCol);
    }
    else
    {
        // not implemented
    }
}

void checkSphereSphere(PhysicsObject *sphere1, ColliderSphere *sphereCol1, PhysicsObject *sphere2, ColliderSphere *sphereCol2)
{
    if (sphere1->mass == 0 && sphere2->mass == 0)
    {
        // not implemented
    }
    else if (sphere1->mass == 0)
    {
        checkDynamicSphereKinematicSphere(sphere2, sphereCol2, sphere1, sphereCol1);
    }
    else if (sphere2->mass == 0)
    {
        checkDynamicSphereKinematicSphere(sphere1, sphereCol1, sphere2, sphereCol2);
    }
    else
    {
        // not implemented
    }
}

void checkDynamicSphereKinematicMesh(PhysicsObject *dSphere, ColliderSphere *dSphereCol, PhysicsObject *kMesh, ColliderMesh *kMeshCol)
{
    mat4 M = glm::translate(glm::mat4(1.f), kMesh->position) * glm::mat4_cast(kMesh->orientation) * glm::scale(glm::mat4(1.f), kMesh->scale);

    // Check bounding spheres
    if (distance2(dSphere->position, kMesh->position) <= pow(dSphereCol->bbox.radius + kMeshCol->bbox.radius, 2))
    {
        // Check faces
        for (int i = 0; i < kMeshCol->mesh->getNumFaces(); i++)
        {
            vector<vec3> v = kMeshCol->mesh->getFace(i, M);

            // Check if sphere is touching triangle
            vec3 normal = normalize(cross(v[1] - v[0], v[2] - v[0]));
            vec3 dir = -normal;
            vec2 bary;
            float d;
            bool rayDidIntersect = intersectRayPlane(dSphere->position, dir, v[0], normal, d);

            if (rayDidIntersect && fabs(d) < dSphereCol->radius)
            {
                rayDidIntersect = intersectRayTriangle(dSphere->position, dir, v[0], v[1], v[2], bary, d);

                if (rayDidIntersect)
                {
                    Collision collision;
                    collision.other = kMesh;
                    collision.normal = dir;
                    collision.penetration = dSphereCol->radius - d;
                    dSphereCol->pendingCollisions.push_back(collision);
                }
                else
                {
                    bool hitEdge = false;
                    for (int i = 0; i < 3; i++)
                    {
                        // check edges
                        int e0 = i;
                        int e1 = (i + 1) % 3;
                        vec3 closestPoint = v[e0] + proj(dSphere->position - v[e0], normalize(v[e1] - v[e0]));
                        d = distance(dSphere->position, closestPoint);

                        if (d < dSphereCol->radius &&
                            dot(v[e1] - v[e0], closestPoint - v[e0]) > 0 && dot(v[e0] - v[e1], closestPoint - v[e1]) > 0)
                        {
                            Collision collision;
                            collision.other = kMesh;
                            collision.normal = normalize(closestPoint - dSphere->position);
                            collision.penetration = dSphereCol->radius - d;
                            dSphereCol->pendingCollisions.push_back(collision);
                            hitEdge = true;
                            break;
                        }
                    }

                    if (!hitEdge)
                    {
                        //check vertices
                        for (int i = 0; i < 3; i++)
                        {
                            d = distance(dSphere->position, v[i]);
                            if (d < dSphereCol->radius)
                            {
                                Collision collision;
                                collision.other = kMesh;
                                collision.normal = normalize(v[0] - dSphere->position);
                                collision.penetration = dSphereCol->radius - d;
                                dSphereCol->pendingCollisions.push_back(collision);
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
}

void checkDynamicSphereKinematicSphere(PhysicsObject *dSphere, ColliderSphere *dSphereCol, PhysicsObject *kSphere, ColliderSphere *kSphereCol)
{
    
}