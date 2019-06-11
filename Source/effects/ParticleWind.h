#pragma once

#include <memory>
#include <cstdlib>
#include <glm/glm.hpp>

#include "../engine/ParticleEmitter.h"
#include "../engine/Collider.h"

class ParticleWind : public Particle
{
public:
    ParticleWind(glm::vec3 origin, glm::quat orientation, float radius, float length);
    virtual void update();

    glm::vec3 origin;
    glm::quat orientation;
    float length;
    float radius;
};