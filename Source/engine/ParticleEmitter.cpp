#include "ParticleEmitter.h"
#include "../MatrixStack.h"
#include "../Texture.h"

#include <memory>

using namespace std;
using namespace glm;

ParticleEmitter::ParticleEmitter(int maxParticles) :
    maxParticles(maxParticles), numActiveParticles(0)
{
}

void ParticleEmitter::update(float dt)
{
    numActiveParticles = 0;
    for (auto &p : particles)
    {
        if (p->t < p->lifespan)
        {
            numActiveParticles++;
            p->update(dt);
        }
    }
}

void ParticleEmitter::init(shared_ptr<Shape> billboard, shared_ptr<Texture> texture)
{
    this->billboard = billboard;
    this->texture = texture;
}

void ParticleEmitter::draw(shared_ptr<Program> prog)
{
    texture->bind(prog->getUniform("alphaTexture"));
    auto M = make_shared<MatrixStack>();
    for (auto &p : particles)
    {
        if (p->t < p->lifespan)
        {
            p->draw(prog, M, billboard);
        }
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

void Particle::draw(shared_ptr<Program> prog, shared_ptr<MatrixStack> M, std::shared_ptr<Shape> billboard)
{
    M->pushMatrix();
        M->translate(position);
        // M->rotate(rotation);
        M->scale(scale);
        glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
        glUniform4fv(prog->getUniform("pColor"), 1, value_ptr(color));
        billboard->draw(prog);
    M->popMatrix();
}

void Particle::update(float dt)
{
    t += dt;
    if (t > lifespan && respawn)
    {
        start();
    }
    velocity += acceleration;
    position += dt * velocity;
    
}

Particle::Particle() :
    respawn(false), t(0), scale(1), rotation(0), acceleration(0), position(0), velocity(0), color(1)
{
}