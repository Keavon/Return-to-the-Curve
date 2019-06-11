#include "ParticleWind.h"

#include "../engine/ParticleEmitter.h"
#include <algorithm>

using namespace std;
using namespace glm;

ParticleWind::ParticleWind(vec3 origin, quat orientation, float radius, float length) : Particle()
{
    vec3 direction = vec3(mat4_cast(orientation) * vec4(0, 1, 0, 0));
    float speed = ((rand() % 100) / 10.0f + 10.0f);
    velocity = direction * speed;
    lifespan = length / speed;
    vec3 offset = normalize(cross(direction, vec3(rand() % 100 - 50, rand() % 100 - 50, rand() % 100 - 50))) *
        ((rand() % 100) / 100.f * radius);
    position = origin + offset;

    acceleration = vec3(0);
    color = vec4(0, 1, 1, 1);
    scale = 0.3f;

    start();
}

void ParticleWind::update()
{
    Particle::update();

    color.a = std::min(1.f, 10.f * (lifespan - t) / lifespan);
}