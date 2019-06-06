#include "Goal.h"

using namespace std;
using namespace glm;

Goal::Goal(vec3 position, quat orientation, shared_ptr<Shape> model, float radius) :
    PhysicsObject(position, orientation, vec3(1, 1, 1), model, make_shared<TriggerSphere>(radius)), radius(radius), ballInGoal(false), didWin(false)
{
}

void Goal::init(shared_ptr<ParticleEmitter> fireworkEmitter, float *startTime)
{
    this->fireworkEmitter = fireworkEmitter;
    this->startTime = startTime;
}

void Goal::update()
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
                dynamic_cast<Ball *>(collision.other)->frozen = 1;
                collision.other->position = position + vec3(0, 1, 0);
            }
        }
    }
    collider->pendingCollisions.clear();
}

void Goal::onWin()
{
    soundEngine->win();

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
    collider->pendingCollisions.clear();
}