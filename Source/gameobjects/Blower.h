#pragma once

#include "../engine/PhysicsObject.h"
#include "../engine/TriggerSphere.h"
#include "../engine/ParticleEmitter.h"
#include "Ball.h"

#include <glm/glm.hpp>
#include <memory>
#include <iostream>

class Blower : public PhysicsObject
{
public:
    Blower(vec3 position, quat orientation, float radius, float length);
    void init(std::shared_ptr<ParticleEmitter> windEmitter);
    virtual void update();
    virtual void triggerStay(PhysicsObject *object);

    float radius;
    float length;
    float force;
    float t;
    std::shared_ptr<ParticleEmitter> windEmitter;
};