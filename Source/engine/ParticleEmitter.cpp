#include "ParticleEmitter.h"

#include <memory>

using namespace std;
using namespace glm;

ParticleEmitter::ParticleEmitter(int maxParticles) : maxParticles(maxParticles)
{
}

void ParticleEmitter::update(float dt)
{
    for (auto &p : particles)
    {
        p->update(dt);
    }
}

void ParticleEmitter::draw(shared_ptr<Program> prog)
{
    for (auto &p : particles)
    {
        p->draw(prog);
    }
}

void ParticleEmitter::addParticle(shared_ptr<Particle> p)
{
    if (particles.size() < maxParticles)
    {
        particles.push_back(p);
    }
    else
    {
        float tMax = particles[0]->t;
        int idx = 0;
        for (int i = 0; i < maxParticles; i++)
        {
            if (particles[i]->t > particles[i]->lifespan)
            {
                particles[i] = p;
                return;
            }
            if (particles[i]->t > tMax)
            {
                tMax = particles[i]->t;
                idx = i;
            }
        }
        particles[idx] = p;
    }
}

void Particle::draw(shared_ptr<Program> prog)
{

}

void Particle::update(float dt)
{
    t += dt;
    if (t > lifespan)
    {
        start();
    }
    velocity += acceleration;
    position += dt * velocity;
    
}