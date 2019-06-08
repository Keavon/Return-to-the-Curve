#pragma once

#include <memory>
#include <cstdlib>
#include <glm/glm.hpp>

#include "../engine/ParticleEmitter.h"
#include "../engine/Collider.h"

using namespace std;
using namespace glm;

class ParticleSpark : public Particle
{
public:
    ParticleSpark(vec3 position, float impactVel, vec3 impactNormal);
    virtual void update();
};