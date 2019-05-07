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

using namespace glm;
using namespace std;

Collider::Collider(PhysicsObject *parent, vec3 min, vec3 max) :
    parent(parent), bbox(min, max), pendingCollision({})
{
}

// Note: assumes that only sphere is affected by interaction
bool checkSphereMesh(ColliderSphere *sphere, ColliderMesh *mesh)
{
    bool hit = false;
    mat4 M = glm::translate(glm::mat4(1.f), mesh->parent->position) * glm::mat4_cast(mesh->parent->orientation);

    // Check bounding spheres
    if (distance2(sphere->parent->position, mesh->parent->position) <= pow(sphere->bbox.radius + mesh->bbox.radius, 2))
    {
        // Check faces
        for (int i = 0; i < mesh->mesh->getNumFaces(); i++)
        {
            vector<vec3> v = mesh->mesh->getFace(i, M);

            // Check if sphere is touching triangle
            vec3 normal = normalize(cross(v[1] - v[0], v[2] - v[0]));
            vec3 dir = -normal;
            vec2 bary;
            float d;
            bool rayDidIntersect = intersectRayTriangle(sphere->parent->position, dir, v[0], v[1], v[2], bary, d);

            if (rayDidIntersect && fabs(d) <= sphere->radius &&
                bary.x > 0 && bary.y > 0 && (1 - bary.x - bary.y) > 0)
            {

                // Find precise collision point
                vec3 faceOffset = normal * sphere->radius;
                dir = -normalize(sphere->parent->velocity);
                rayDidIntersect = intersectRayTriangle(sphere->parent->position, dir,
                    v[0]+faceOffset, v[1]+faceOffset, v[2]+faceOffset, bary, d);

                if (rayDidIntersect)
                {
                    // Check if this is the soonest collision
                    float t = d / length(sphere->parent->velocity);
                    if (fabs(t) > fabs(sphere->pendingCollision.t))
                    {
                        hit = true;
                        sphere->pendingCollision.hit = true;
                        sphere->pendingCollision.normal = normal;
                        sphere->pendingCollision.point = v[0] * (1 - bary.x - bary.y) + v[1] * bary.x + v[2] * bary.y;
                        sphere->pendingCollision.position = sphere->pendingCollision.point + faceOffset;
                        sphere->pendingCollision.t = t;
                    }
                }
            }

            // Check edges
            for (int i = 0; i < mesh->mesh->getNumEdges(); i++)
            {
                vector<vec3> v = mesh->mesh->getEdge(i, M);

                // Check if sphere is touching edge
                vec3 closestPoint = v[0] + proj(sphere->parent->position - v[0], normalize(v[1] - v[0]));
                if (distance2(closestPoint, sphere->parent->position) < pow(sphere->radius, 2) &&
                    dot(v[1] - v[0], closestPoint - v[0]) > 0 && dot(v[0] - v[1], closestPoint - v[1]) > 0)
                {
                    // Find precise collision point
                    vec3 dir = -normalize(sphere->parent->velocity);
                    vec3 intersect;
                    vec3 normal;
                    bool rayDidIntersect = intersectRayCylinder(sphere->parent->position, dir, v[0], v[1],
                        sphere->radius, intersect, normal);
                    if (rayDidIntersect)
                    {
                        // Check if this is the soonest collision
                        float t = distance(sphere->parent->position, intersect) / length(sphere->parent->velocity);
                        if (fabs(t) > fabs(sphere->pendingCollision.t))
                        {
                            hit = true;
                            sphere->pendingCollision.hit = true;
                            sphere->pendingCollision.normal = normal;
                            sphere->pendingCollision.point = closestPoint;
                            sphere->pendingCollision.position = intersect;
                            sphere->pendingCollision.t = t;
                        }
                    }

                }
            }

            // Check vertices
            for (int i = 0; i < mesh->mesh->getNumVertices(); i++)
            {
                vec3 v = mesh->mesh->getVertex(i, M);

                // Check if sphere is touching vertex
                if (distance2(sphere->parent->position, v) < pow(sphere->radius, 2))
                {
                    // Find precise collision point
                    vec3 dir = -normalize(sphere->parent->velocity);
                    vec3 intersect;
                    vec3 normal;
                    bool rayDidIntersect = intersectRaySphere(sphere->parent->position, dir, v, sphere->radius, intersect, normal);
                    if (rayDidIntersect)
                    {
                        // Check if this is the soonest collision
                        float t = distance(sphere->parent->position, intersect) / length(sphere->parent->velocity);
                        if (fabs(t) > fabs(sphere->pendingCollision.t))
                        {
                            hit = true;
                            sphere->pendingCollision.hit = true;
                            sphere->pendingCollision.normal = normal;
                            sphere->pendingCollision.point = v;
                            sphere->pendingCollision.position = intersect;
                            sphere->pendingCollision.t = t;
                        }
                    }
                }
            }
        }
    }

    return hit;
}