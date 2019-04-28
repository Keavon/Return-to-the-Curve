#pragma once

#include "GameObject.h"
#include "Collider.h"
#include "../Shape.h"
#include <glm/glm.hpp>
#include <memory>

// https://gafferongames.com/post/physics_in_3d/

class PhysicsObject : public GameObject
{
public:
    PhysicsObject(glm::vec3 position, glm::quat orientation, std::shared_ptr<Shape> model, std::shared_ptr<Collider> collider);
    float friction;
    glm::vec3 velocity;
    glm::vec3 acceleration;
    float speed;
    float mass;
    std::shared_ptr<Collider> collider;
    glm::vec3 netForce; // net forces acting on ball, calculated each frame
};