#include "Goal.h"

Goal::Goal(vec3 position, quat orientation, shared_ptr<Shape> model, float radius) :
    PhysicsObject(position, orientation, vec3(1, 1, 1), model, make_shared<TriggerSphere>(radius)), radius(radius), didWin(false)
{
}

void Goal::init(shared_ptr<ParticleEmitter> fireworkEmitter, float *startTime)
{
    this->fireworkEmitter = fireworkEmitter;
    this->startTime = startTime;
}

void Goal::update()
{
    clearCollisions();
}

void Goal::triggerEnter(PhysicsObject *object)
{
    auto ball = dynamic_cast<Ball *>(object);
    if (ball != NULL)
    {
        didWin = true;
        soundEngine->win();
        ball->frozen = 1;
        ball->position = position + vec3(0, 1, 0);

        for (int i = 0; i < 100; i++)
        {
            fireworkEmitter->addParticle(make_shared<ParticleFirework>(position));
        }
    }
}

void Goal::reset()
{
    didWin = false;
    fireworkEmitter->stop();
    clearCollisions();
}