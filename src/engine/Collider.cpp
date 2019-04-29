#include "Collider.h"

#include "ColliderSphere.h"
#include "ColliderMesh.h"
#include "PhysicsObject.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/intersect.hpp>
#include <glm/gtx/norm.hpp>
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
            vector<vec3> v = mesh->mesh->getFace(i);
            for (int j = 0; j < v.size(); j++)
            {
                v[j] = vec3(M * vec4(v[j], 1.0f));
            }

            // Check if sphere is touching triangle
            vec3 normal = normalize(cross(v[1] - v[0], v[2] - v[0]));
            vec3 dir = -normal;
            vec2 bary = vec2(NAN);
            float d = NAN;
            intersectRayTriangle(sphere->parent->position, dir, v[0], v[1], v[2], bary, d);

            if (!isnan(d) && fabs(d) <= sphere->radius)
            {

                // Find precise collision point
                vec3 faceOffset = normal * sphere->radius;
                dir = -normalize(sphere->parent->velocity);
                bary = vec2(NAN);
                d = NAN;
                intersectRayTriangle(sphere->parent->position, dir, v[0]+faceOffset, v[1]+faceOffset, v[2]+faceOffset, bary, d);

                if (!isnan(d))
                {
                    // Check if this is the soonest collision
                    float t = d / length(sphere->parent->velocity);
                    if (!sphere->pendingCollision.hit || fabs(t) > fabs(sphere->pendingCollision.t))
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
        }

        // Check edges

        // Check vertices
    }

    return hit;
}