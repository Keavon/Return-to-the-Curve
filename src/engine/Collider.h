#pragma once

#include "BoundingBox.h"

#include <glm/glm.hpp>

// https://eli.thegreenplace.net/2016/a-polyglots-guide-to-multiple-dispatch/

class ColliderMesh;
class ColliderSphere;

class Collider
{
public:
    Collider(glm::vec3 *position, glm::quat *orientation, glm::vec3 min, glm::vec3 max);

    virtual bool checkCollision(Collider *col) = 0;
    virtual bool checkCollision(ColliderMesh *col) {};
    virtual bool checkCollision(ColliderSphere *col) {};

    BoundingBox bbox;
    glm::vec3 *position;
    glm::quat *orientation;
};