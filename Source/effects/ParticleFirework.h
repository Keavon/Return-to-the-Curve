#pragma once

#include <memory>
#include <glm/glm.hpp>
#include <cstdlib>

#include "../engine/ParticleEmitter.h"
#include "../engine/Collider.h"

using namespace std;
using namespace glm;

class ParticleFirework : public Particle
{
public:
    ParticleFirework(vec3 startPosition);
    virtual void start();
    virtual void update();
};