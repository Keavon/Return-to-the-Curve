#pragma once

#include "GameObject.h"
#include <glm/glm.hpp>

class PhysicsObject : public GameObject
{
public:
    bool testCollision(PhysicsObject other);
    float friction;
    glm::vec3 velocity;
    glm::vec3 acceleration;
    float speed;
    float mass;
    Collider collider;
    glm::vec3 netForce; // net forces acting on ball, calculated each frame
};