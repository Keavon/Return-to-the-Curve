#include "Goal.h"
#include "../engine/TriggerSphere.h"
#include "../engine/ParticleEmitter.h"
#include "../effects/ParticleFirework.h"
#include "Ball.h"

#include <glm/glm.hpp>
#include <memory>
#include <iostream>

using namespace std;
using namespace glm;


Goal::Goal(vec3 position, quat orientation, shared_ptr<Shape> model, float radius) :
    PhysicsObject(position, orientation, model, make_shared<TriggerSphere>(radius)),
    radius(radius), ballInGoal(false), didWin(false)
{

}

void Goal::init(shared_ptr<ParticleEmitter> fireworkEmitter, float *startTime)
{
    this->fireworkEmitter = fireworkEmitter;
    this->startTime = startTime;
}

void Goal::update(float dt)
{
    for (auto collision : collider->pendingCollisions)
    {
        if (dynamic_cast<Ball *>(collision.other) != NULL)
        {
            ballInGoal = true;
            if (!didWin)
            {
                didWin = true;
                onWin();
            }
        }
    }
    collider->pendingCollisions.clear();
}

void Goal::onWin()
{
    cout << "✼　 ҉ 　✼　 ҉ 　✼" << endl;
    cout << "You win!" << endl;
    cout << "Time: " << glfwGetTime() - *startTime << endl;
    cout << "✼　 ҉ 　✼　 ҉ 　✼" << endl;

    for (int i = 0; i < 100; i++)
    {
        fireworkEmitter->addParticle(make_shared<ParticleFirework>(position));
    }
}

void Goal::reset()
{
    ballInGoal = false;
    didWin = false;
    fireworkEmitter->stop();
}