#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/projection.hpp>
#include <glm/gtx/intersect.hpp>
#include <cmath>
#include <iostream>
#include <unordered_set>
#include <memory>
#include <glm/glm.hpp>
#include <vector>

#include "../effects/ParticleSpark.h"
#include "../engine/Material.h"
#include "../effects/Sound.h"
#include "../engine/PhysicsObject.h"
#include "../engine/ParticleEmitter.h"
#include "../engine/ColliderSphere.h"
#include "../engine/Time.h"
#include "../Shape.h"
#include "../WindowManager.h"
#include "Enemy.h"

class Ball : public PhysicsObject
{
public:
    Ball(glm::vec3 position, glm::quat orientation, std::shared_ptr<Shape> model, float radius);
    void init(WindowManager *windowManager, std::shared_ptr<ParticleEmitter> sparkEmitter);
    void update(glm::vec3 dolly, glm::vec3 strafe);
    virtual void onHardCollision(float impactVel, Collision &collision);
    void addSkin(std::shared_ptr<Material> newSkin);
    void setSkin(int skinIndex);
    void nextSkin();
    std::shared_ptr<Material> getSkinMaterial();

	WindowManager *windowManager;
    std::shared_ptr<ParticleEmitter> sparkEmitter;
    float radius;
    float moveForce;
    float jumpForce;
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