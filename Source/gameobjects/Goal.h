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
    Goal(glm::vec3 position, glm::quat orientation, std::shared_ptr<Shape> model, float radius);
    void update();
    void init(std::shared_ptr<ParticleEmitter> fireworkEmitter, float *startTime);
    void onWin();
    void reset();

    float radius;
    std::shared_ptr<ParticleEmitter> fireworkEmitter;
    bool ballInGoal;
    bool didWin;
    float *startTime;
};