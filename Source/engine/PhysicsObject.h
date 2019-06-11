#pragma once

#include <memory>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <unordered_set>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/projection.hpp>
#include <glm/gtx/intersect.hpp>
#include <glm/glm.hpp>

#include "GameObject.h"
#include "Collider.h"
#include "ColliderSphere.h"
#include "Collider.h"
#include "Time.h"
#include "../Shape.h"
#include "../effects/Sound.h"

#define GRAVITY -50.0f
#define DRAG_COEFFICIENT 0.25f

using namespace std;
using namespace glm;

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
    vec3 velocity;
    vec3 acceleration;
    shared_ptr<Collider> collider;
    vec3 impulse;
    vec3 normForce;
    vec3 netForce; // net forces acting on ball, calculated each frame

public:
	PhysicsObject();
    PhysicsObject(vec3 position, shared_ptr<Shape> model, shared_ptr<Collider> collider = nullptr);
    PhysicsObject(vec3 position, quat orientation, shared_ptr<Shape> model, shared_ptr<Collider> collider = nullptr);
    PhysicsObject(vec3 position, quat orientation, vec3 scale, shared_ptr<Shape> model, shared_ptr<Collider> collider = nullptr);

    // standard interface
    /* GameObject.h: virtual void draw(shared_ptr<Program> prog, shared_ptr<MatrixStack> M)); */
    virtual void start();
    virtual void update();
    virtual void lateUpdate();
    virtual void physicsUpdate();
    virtual void latePhysicsUpdate();
    virtual void triggerEnter(PhysicsObject *object);
    virtual void triggerStay(PhysicsObject *object);
    virtual void triggerExit(PhysicsObject *object);
    virtual void onHardCollision(float impactVel, Collision &collision);

    void checkCollision(PhysicsObject *other);
    void clearCollisions();
    float getRadius(); // get radius of bounding sphere
    void applyImpulse(vec3 impulse);
    void setMass(float mass);
    void setFriction(float friction);
    void setElasticity(float elasticity);
    void setVelocity(vec3 velocity);
    vec3 getCenterPos();
    vec3 getVelocity();
    bool ignoreCollision;
    bool solid;
};