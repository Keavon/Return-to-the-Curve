#include "Collider.h"

#include "ColliderSphere.h"
#include "ColliderMesh.h"
#include "PhysicsObject.h"
#include "Raytrace.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/intersect.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/projection.hpp>
#include <glm/glm.hpp>
#include <vector>
#include <cmath>
#include <iostream>
#include <algorithm>

using namespace glm;
using namespace std;

Collider::Collider(vec3 min, vec3 max) :
    bbox(min, max), pendingCollision({})
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
    mat4 M = glm::translate(glm::mat4(1.f), kMesh->position) * glm::mat4_cast(kMesh->orientation);

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
            bool rayDidIntersect = intersectRayTriangle(dSphere->position, dir, v[0], v[1], v[2], bary, d);

            if (rayDidIntersect && fabs(d) <= dSphereCol->radius &&
                bary.x > 0 && bary.y > 0 && (1 - bary.x - bary.y) > 0)
            {

                // Find precise collision point
                vec3 faceOffset = normal * dSphereCol->radius;
                dir = -normalize(dSphere->velocity);
                rayDidIntersect = intersectRayTriangle(dSphere->position, dir,
                    v[0]+faceOffset, v[1]+faceOffset, v[2]+faceOffset, bary, d);

                if (rayDidIntersect)
                {
                    // Check if this is the soonest collision
                    float t = d / length(dSphere->velocity);
                    if (fabs(t) > fabs(dSphereCol->pendingCollision.t))
                    {
                        dSphereCol->pendingCollision.hit = true;
                        dSphereCol->pendingCollision.normal = normal;
                        dSphereCol->pendingCollision.point = v[0] * (1 - bary.x - bary.y) + v[1] * bary.x + v[2] * bary.y;
                        dSphereCol->pendingCollision.position = dSphereCol->pendingCollision.point + faceOffset;
                        dSphereCol->pendingCollision.t = t;
                    }
                }
            }

            // Check edges
            for (int i = 0; i < kMeshCol->mesh->getNumEdges(); i++)
            {
                vector<vec3> v = kMeshCol->mesh->getEdge(i, M);

                // Check if sphere is touching edge
                vec3 closestPoint = v[0] + proj(dSphere->position - v[0], normalize(v[1] - v[0]));
                if (distance2(closestPoint, dSphere->position) < pow(dSphereCol->radius, 2) &&
                    dot(v[1] - v[0], closestPoint - v[0]) > 0 && dot(v[0] - v[1], closestPoint - v[1]) > 0)
                {
                    // Find precise collision point
                    vec3 dir = -normalize(dSphere->velocity);
                    vec3 intersect;
                    vec3 normal;
                    bool rayDidIntersect = intersectRayCylinder(dSphere->position, dir, v[0], v[1],
                        dSphereCol->radius, intersect, normal);
                    if (rayDidIntersect)
                    {
                        // Check if this is the soonest collision
                        float t = distance(dSphere->position, intersect) / length(dSphere->velocity);
                        if (fabs(t) > fabs(dSphereCol->pendingCollision.t))
                        {
                            dSphereCol->pendingCollision.hit = true;
                            dSphereCol->pendingCollision.normal = normal;
                            dSphereCol->pendingCollision.point = closestPoint;
                            dSphereCol->pendingCollision.position = intersect;
                            dSphereCol->pendingCollision.t = t;
                        }
                    }

                }
            }

            // Check vertices
            for (int i = 0; i < kMeshCol->mesh->getNumVertices(); i++)
            {
                vec3 v = kMeshCol->mesh->getVertex(i, M);

                // Check if sphere is touching vertex
                if (distance2(dSphere->position, v) < pow(dSphereCol->radius, 2))
                {
                    // Find precise collision point
                    vec3 dir = -normalize(dSphere->velocity);
                    vec3 intersect;
                    vec3 normal;
                    bool rayDidIntersect = intersectRaySphere(dSphere->position, dir, v, dSphereCol->radius, intersect, normal);
                    if (rayDidIntersect)
                    {
                        // Check if this is the soonest collision
                        float t = distance(dSphere->position, intersect) / length(dSphere->velocity);
                        if (fabs(t) > fabs(dSphereCol->pendingCollision.t))
                        {
                            dSphereCol->pendingCollision.hit = true;
                            dSphereCol->pendingCollision.normal = normal;
                            dSphereCol->pendingCollision.point = v;
                            dSphereCol->pendingCollision.position = intersect;
                            dSphereCol->pendingCollision.t = t;
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