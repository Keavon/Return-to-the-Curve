#include "ParticleFirework.h"

#include <glm/glm.hpp>
#include <memory>
#include <cstdlib>

using namespace std;
using namespace glm;

ParticleFirework::ParticleFirework(vec3 startPosition) : Particle()
{
    this->startPosition = startPosition;
    start();
}

void ParticleFirework::start()
{
    Particle::start();

    position = startPosition;
    velocity = normalize(vec3(rand() % 100 - 50, rand() % 50, rand() % 100 - 50)) * (float) ((rand() % 100) * 0.2 + 10);
    acceleration = -velocity * 0.5f;
    lifespan = (rand() % 100) * 0.005 + 0.5;
    rotationSpeed = ((rand() % 628) - 314) * 0.01;

    color = vec4(1, 0, 0, 1);
    scale = 0.3;
    respawn = true;
}

void ParticleFirework::update(float dt)
{
    Particle::update(dt);

    color.a = (lifespan - t) / lifespan;
    color.g = t / lifespan;
    scale = 0.3 + t / lifespan;
}