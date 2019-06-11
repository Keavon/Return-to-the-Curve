#include "ParticleEmitter.h"

ParticleEmitter::ParticleEmitter(shared_ptr<Shape> billboard, shared_ptr<Texture> texture, int maxParticles)
{
    this->billboard = billboard;
    this->texture = texture;
    this->maxParticles = maxParticles;
    this->numActiveParticles = 0;
}

void ParticleEmitter::update()
{
    numActiveParticles = 0;
    for (auto &p : particles)
    {
        if (p->t < p->lifespan)
        {
            numActiveParticles++;
            p->update();
        }
    }
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

void ParticleEmitter::stop()
{
    numActiveParticles = 0;
    particles.clear();
}

void Particle::draw(shared_ptr<Program> prog, shared_ptr<MatrixStack> M, shared_ptr<Shape> billboard)
{
    M->pushMatrix();
        M->translate(position);
        // M->rotate(rotation, vec3(0, 0, 1));
        M->scale(scale);
        glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
        glUniform4fv(prog->getUniform("pColor"), 1, value_ptr(color));
        billboard->draw(prog);
    M->popMatrix();
}

void Particle::update()
{
    t += Time.deltaTime;
    if (t > lifespan && respawn) start();
    velocity += Time.deltaTime * acceleration;
    position += Time.deltaTime * velocity;
    rotation += Time.deltaTime * rotationSpeed;
}

Particle::Particle() : respawn(false), t(0), scale(1), rotation(0), acceleration(0), position(0), velocity(0), color(1), rotationSpeed(0)
{
}

void Particle::start()
{
    t = 0;
}