#include "Blower.h"

#include <memory>
#include "../engine/ParticleEmitter.h"
#include "../effects/ParticleWind.h"
#include "../engine/Time.h"
#include <math.h>
#include <glm/glm.hpp>

using namespace glm;
using namespace std;

Blower::Blower(vec3 position, quat orientation, float radius, float length) :
    PhysicsObject(position, orientation, nullptr, make_shared<TriggerCylinder>(radius, length)),
    radius(radius), length(length)
{
    t = 0;
    force = 50;
}

void Blower::update()
{
    t += Time.physicsDeltaTime;
    float volume = (float)M_PI * radius * radius * length;
    float particleInterval = 2.f / volume;
    while (t > particleInterval)
    {
        t -= particleInterval;
        windEmitter->addParticle(make_shared<ParticleWind>(position, orientation, radius, length));
    }
    clearCollisions();
}

void Blower::triggerStay(PhysicsObject *object)
{
    object->applyImpulse(vec3(mat4_cast(orientation) * vec4(0, 1, 0, 0)) * force);
}

void Blower::init(shared_ptr<ParticleEmitter> windEmitter)
{
    this->windEmitter = windEmitter;
}