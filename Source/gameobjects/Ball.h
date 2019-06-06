#pragma once

#include "../engine/PhysicsObject.h"
#include "../engine/ParticleEmitter.h"
#include "../engine/Material.h"
#include "../Shape.h"
#include "../WindowManager.h"
#include "PowerUp.h"
#include <memory>
#include <glm/glm.hpp>
#include <vector>

class Ball : public PhysicsObject
{
public:
    Ball(glm::vec3 position, glm::quat orientation, std::shared_ptr<Shape> model, float radius);
    void init(WindowManager *windowManager, std::shared_ptr<ParticleEmitter> sparkEmitter);
    void update(float dt, glm::vec3 dolly, glm::vec3 strafe);
    void activatePowerUp();
    void prepNextPowerUp();
    virtual void onHardCollision(float impactVel, Collision &collision);
    void addSkin(std::shared_ptr<Material> newSkin);
    void setSkin(int skinIndex);
    void nextSkin();
    std::shared_ptr<Material> getSkinMaterial();

	WindowManager *windowManager;
    PowerUp *activePowerUp;
    std::shared_ptr<ParticleEmitter> sparkEmitter;
    float radius;
    float moveForce;
    float jumpForce;
    std::vector<PowerUp *> storedPowerUp;
    bool hasPowerUp;
    bool powerUpReady;
    bool frozen;

    float JUMP_TIME;
    int WANTS_JUMP;

    float LAND_TIME;
    int CAN_JUMP;
    glm::vec3 LAST_NORMAL_FORCE;

    float JUMPED_AT_TIME;
    int JUST_JUMPED;
    glm::vec3 startPosition;
    glm::vec3 playPosition;
    int currentSkin = 0;
    std::vector<std::shared_ptr<Material>> marbleSkins;
};