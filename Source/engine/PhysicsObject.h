#pragma once

#include "GameObject.h"
#include "Collider.h"
#include "../Shape.h"
#include <glm/glm.hpp>
#include <memory>
#include <irrKlang.h> // audio lib

#define GRAVITY -30.0f
#define DRAG_COEFFICIENT 0.25f

// https://gafferongames.com/post/physics_in_3d/

class PhysicsObject : public GameObject
{
public:
    PhysicsObject(glm::vec3 position, glm::quat orientation, std::shared_ptr<Shape> model, std::shared_ptr<Collider> collider);
	PhysicsObject();
    void resolveCollision(float dt);
    void checkCollision(PhysicsObject *other);
    void update(float dt);
    float getRadius(); // get radius of bounding sphere

    float friction;
    float elasticity;
    glm::vec3 velocity;
    glm::vec3 acceleration;
    float speed;
    float mass;
    float invMass;
    std::shared_ptr<Collider> collider;
    glm::vec3 netForce; // net forces acting on ball, calculated each frame
    glm::vec3 normForce;
    glm::vec3 impulse;
};