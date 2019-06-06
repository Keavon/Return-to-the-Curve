#pragma once

#include "GameObject.h"
#include "Collider.h"
#include "../Shape.h"
#include "../effects/Sound.h"
#include "Time.h"
#include <glm/glm.hpp>
#include <memory>

#define GRAVITY -50.0f
#define DRAG_COEFFICIENT 0.25f

// https://gafferongames.com/post/physics_in_3d/

class PhysicsObject : public GameObject
{
private:
    // Physical properties
    float mass;
    float friction;
    float elasticity;

protected:
    float invMass;
    float speed;
    glm::vec3 velocity;
    glm::vec3 acceleration;
    std::shared_ptr<Collider> collider;
    glm::vec3 impulse;
    glm::vec3 normForce;
    glm::vec3 netForce; // net forces acting on ball, calculated each frame

public:
	PhysicsObject();
    PhysicsObject(glm::vec3 position, std::shared_ptr<Shape> model, std::shared_ptr<Collider> collider = nullptr);
    PhysicsObject(glm::vec3 position, glm::quat orientation, std::shared_ptr<Shape> model, std::shared_ptr<Collider> collider = nullptr);
    PhysicsObject(glm::vec3 position, glm::quat orientation, glm::vec3 scale, std::shared_ptr<Shape> model, std::shared_ptr<Collider> collider = nullptr);
    void resolveCollision();
    void checkCollision(PhysicsObject *other);
    void update();
    virtual void onHardCollision(float impactVel, Collision &collision);
    float getRadius(); // get radius of bounding sphere
    void applyImpulse(glm::vec3 impulse);
    void setMass(float mass);
    void setFriction(float friction);
    void setElasticity(float elasticity);
    void setVelocity(glm::vec3 velocity);
    glm::vec3 getCenterPos();
    glm::vec3 getVelocity();
};