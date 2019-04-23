#include "ColliderMesh.h"

#include "BoundingBox.h"
#include <memory>
#include <glm/glm.hpp>
#include "../Shape.h"

using namespace glm;
using namespace std;

ColliderMesh::ColliderMesh() : mesh(NULL)
{
    bbox = BoundingBox(vec3(0), vec3(0));
}

ColliderMesh::ColliderMesh(shared_ptr<Shape> mesh) : mesh(mesh)
{
    bbox = BoundingBox(mesh->min, mesh->max);
}