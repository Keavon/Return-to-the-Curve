#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "../Program.h"
#include "../MatrixStack.h"
#include "../Shape.h"
#include "../Texture.h"
#include <memory>

class Particle
{
public:
    Particle();
    virtual void update(float dt);
    virtual void start();
    void draw(std::shared_ptr<Program> prog, std::shared_ptr<MatrixStack> M, std::shared_ptr<Shape> billboard);

    glm::vec3 startPosition;
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 acceleration;
    glm::vec4 color;
    float t;
    float scale;
    float rotation;
    float rotationSpeed;
    float lifespan;
    bool respawn;
};

class ParticleEmitter
{
public:
    ParticleEmitter(int maxParticles);
    void init(std::shared_ptr<Shape> billboard, std::shared_ptr<Texture> texture);
    void update(float dt);
    void draw(std::shared_ptr<Program> prog);
    void addParticle(std::shared_ptr<Particle> p);
    void stop();

    int maxParticles;
    int numActiveParticles;
    std::vector<std::shared_ptr<Particle>> particles;

private:
    std::shared_ptr<Shape> billboard;
    std::shared_ptr<Texture> texture;
};
