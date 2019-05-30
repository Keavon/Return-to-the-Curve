#pragma once

#include "../engine/PhysicsObject.h"
#include "../engine/ParticleEmitter.h"
#include <memory>
#include <glm/glm.hpp>

class Goal : public PhysicsObject
{
public:
    Goal(glm::vec3 position, glm::quat orientation, std::shared_ptr<Shape> model, float radius);
    void update(float dt);
    void init(std::shared_ptr<ParticleEmitter> fireworkEmitter, float *startTime);
    void onWin();
    void reset();

    float radius;
    std::shared_ptr<ParticleEmitter> fireworkEmitter;
    bool ballInGoal;
    bool didWin;
    float *startTime;

    std::shared_ptr<Sound> soundEngine;
};