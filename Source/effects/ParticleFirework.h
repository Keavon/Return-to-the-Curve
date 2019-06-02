#pragma once

#include "../engine/ParticleEmitter.h"
#include <glm/glm.hpp>
#include "../engine/Collider.h"
#include <memory>

class ParticleFirework : public Particle
{
public:
    ParticleFirework(glm::vec3 startPosition);
    virtual void start();
    virtual void update(float dt);
};