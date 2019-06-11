#pragma once

#include <memory>
#include <glm/glm.hpp>
#include <memory>
#include <iostream>

#include "../engine/PhysicsObject.h"
#include "../engine/ParticleEmitter.h"
#include "../engine/TriggerSphere.h"
#include "../effects/ParticleFirework.h"
#include "../effects/Sound.h"
#include "../engine/Time.h"
#include "Ball.h"

using namespace std;
using namespace glm;

class Goal : public PhysicsObject
{
public:
    Goal(vec3 position, quat orientation, shared_ptr<Shape> model, float radius);
    virtual void update();
    void init(shared_ptr<ParticleEmitter> fireworkEmitter, float *startTime);
    virtual void triggerEnter(PhysicsObject *object);
    void reset();

    float radius;
    shared_ptr<ParticleEmitter> fireworkEmitter;
    bool didWin;
    float *startTime;
};