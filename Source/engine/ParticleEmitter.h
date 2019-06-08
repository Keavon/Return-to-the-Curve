#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <iostream>

#include "../Program.h"
#include "../MatrixStack.h"
#include "../Shape.h"
#include "../Texture.h"
#include "Time.h"

using namespace std;
using namespace glm;

class Particle
{
public:
    Particle();
    virtual void update();
    virtual void start();
    void draw(shared_ptr<Program> prog, shared_ptr<MatrixStack> M, shared_ptr<Shape> billboard);

    vec3 startPosition;
    vec3 position;
    vec3 velocity;
    vec3 acceleration;
    vec4 color;
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
    ParticleEmitter(shared_ptr<Shape> billboard, shared_ptr<Texture> texture, int maxParticles);
    void update();
    void draw(shared_ptr<Program> prog);
    void addParticle(shared_ptr<Particle> p);
    void stop();

    int maxParticles;
    int numActiveParticles;
    vector<shared_ptr<Particle>> particles;

private:
    shared_ptr<Shape> billboard;
    shared_ptr<Texture> texture;
};
