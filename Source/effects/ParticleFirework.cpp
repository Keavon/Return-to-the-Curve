#include "ParticleFirework.h"

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
    lifespan = (rand() % 100) * 0.005f + 0.5f;
    rotationSpeed = ((rand() % 628) - 314) * 0.01f;

    color = vec4(1, 0, 0, 1);
    scale = 0.3f;
    respawn = true;
}

void ParticleFirework::update()
{
    Particle::update();

    color.a = (lifespan - t) / lifespan;
    color.g = t / lifespan;
    scale = 0.3f + t / lifespan;
}