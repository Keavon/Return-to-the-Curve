#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "../Program.h"
#include <memory>

class Particle
{
public:
    virtual void update(float dt);
    virtual void start() = 0;
    void draw(std::shared_ptr<Program> prog);

    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 acceleration;
    
    float t;
    float scale;
    float rotation;
    float lifespan;
};

class ParticleEmitter
{
public:
    ParticleEmitter(int maxParticles);
    void update(float dt);
    void draw(std::shared_ptr<Program> prog);
    void addParticle(std::shared_ptr<Particle> p);

    int maxParticles;
    std::vector<std::shared_ptr<Particle>> particles;
};
