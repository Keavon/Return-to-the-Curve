#include "ParticleSpark.h"

#include <glm/glm.hpp>
#include <memory>
#include <cstdlib>

using namespace std;
using namespace glm;

ParticleSpark::ParticleSpark(vec3 position, float impactVel, vec3 impactNormal) : Particle()
{
    this->position = position;
    velocity = normalize(vec3(rand() % 100 - 50, rand() % 100 - 50, rand() % 100 - 50)) * (float) ((rand() % 100) * 0.1 + 1);
    acceleration = -velocity * 0.5f;
    lifespan = (rand() % 100) * 0.01 + 1;

    color = vec4(1, 1, 0, 1);
    scale = 0.25;
}

void ParticleSpark::update(float dt)
{
    Particle::update(dt);
}