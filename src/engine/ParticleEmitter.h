#pragma once

#include <glm/glm.hpp>
#include <vector>

class Particle
{
public:
    void update(float dt);
    void draw();

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
    void draw();
    void addParticle(const Particle &p);

    int maxParticles;
    std::vector<Particle> particles;
};
