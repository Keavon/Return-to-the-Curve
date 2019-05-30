#pragma once

#include "../engine/ParticleEmitter.h"
#include <glm/glm.hpp>
#include "../engine/Collider.h"
#include <memory>

class ParticleSpark : public Particle
{
public:
    ParticleSpark(glm::vec3 position, float impactVel, glm::vec3 impactNormal);
    virtual void update(float dt);
};