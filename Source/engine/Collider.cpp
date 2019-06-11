#include "Collider.h"

#include "ColliderSphere.h"
#include "ColliderMesh.h"
#include "PhysicsObject.h"
#include "../MatrixStack.h"
#include "../gameobjects/Ball.h"

Collider::Collider(vec3 min, vec3 max) :
    bbox(min, max)
{
}

Collider::Collider(float radius) :
    bbox(radius)
{
}

void Collider::clearCollisions(PhysicsObject *owner)
{
    pendingCollisions.clear();
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
        collision1.geom = SPHERE;
        collision1.pos = sphere2->position + collision1.normal * sphere2->getRadius();
        sphereCol1->pendingCollisions.push_back(collision1);

        Collision collision2;
        collision2.other = sphere1;
        collision2.normal = collision1.normal;
        collision2.penetration = collision1.penetration;
        collision2.geom = SPHERE;
        collision2.pos = collision1.pos;
        sphereCol2->pendingCollisions.push_back(collision2);
    }
}



void checkSphereMesh(PhysicsObject *sphere, ColliderSphere *sphereCol, PhysicsObject *mesh, ColliderMesh *meshCol)
{

    // Check bounding spheres
    if (distance2(sphere->getCenterPos(), mesh->getCenterPos()) <= pow(sphere->getRadius() + mesh->getRadius(), 2))
    {
        mat4 M = translate(mat4(1.f), mesh->position) * mat4_cast(mesh->orientation) * scale(mat4(1.f), mesh->scale);

        unordered_set<Edge, EdgeHash> edgeSet;
        unordered_set<vec3> vertSet;
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
                collision.geom = FACE;
                collision.v[0] = v[0];
                collision.v[1] = v[1];
                collision.v[2] = v[2];
                collision.pos = sphere->position + collision.normal * d;
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
                collision.geom = EDGE;
                collision.pos = closestPoint;
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
                collision.geom = VERT;
                collision.pos = v;
                sphereCol->pendingCollisions.push_back(collision);
            }
        }
    }
}