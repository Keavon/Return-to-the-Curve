#include "PowerUp.h"


PowerUp::PowerUp(vec3 position, quat orientation, shared_ptr<Shape> model, float radius, string powerUpType) :
    PhysicsObject(position, orientation, model, make_shared<TriggerSphere>(radius)),
    radius(radius), length(length), powerUpType(powerUpType)
{

}

void PowerUp::update()
{
    clearCollisions();

    if ((float)glfwGetTime() - hiddenTime >= 5)
    {
        hidden = false;
    }

    orientation *= angleAxis(radians(90.0f * Time.deltaTime), vec3(0, 1, 0));
}

void PowerUp::triggerEnter(PhysicsObject *object) {
    if (hidden) return;

    auto ball = dynamic_cast<Ball *>(object);
    if (ball != NULL) ball->collectedPowerUp(powerUpType);
    hidden = true;
    hiddenTime = (float)glfwGetTime();
}

void PowerUp::init(){
}
