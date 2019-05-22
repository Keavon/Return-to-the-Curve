#include "ParticleEmitter.h"

ParticleEmitter::ParticleEmitter(int maxParticles) : maxParticles(maxParticles)
{
}

void ParticleEmitter::update(float dt)
{
    for (auto &p : particles)
    {
        p.update(dt);
    }
}

void ParticleEmitter::addParticle(const Particle &p)
{
    if (particles.size() < maxParticles)
    {
        particles.push_back(p);
    }
    else
    {
        float tMax = particles[0].t;
        int idx = 0;
        for (int i = 0; i < maxParticles; i++)
        {
            if (particles[i].t > particles[i].lifespan)
            {
                particles[i] = p;
                return;
            }
            if (particles[i].t > tMax)
            {
                
            }

        }

        
    }
}