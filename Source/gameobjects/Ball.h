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
#include "PowerUp.h"

using namespace glm;
using namespace std;

class Camera;

class Ball : public PhysicsObject
{
public:
    Ball(vec3 position, quat orientation, shared_ptr<Shape> model, float radius);
    void init(WindowManager *windowManager, shared_ptr<ParticleEmitter> sparkEmitter, shared_ptr<Camera> camera);
    virtual void update();
    virtual void onHardCollision(float impactVel, Collision &collision);
    void addSkin(shared_ptr<Material> newSkin);
    void setSkin(int skinIndex);
    void nextSkin();
    void activatePowerUp();
    void prepNextPowerUp();
    shared_ptr<Material> getSkinMaterial();

	WindowManager *windowManager;
    PowerUp *activePowerUp;
    shared_ptr<ParticleEmitter> sparkEmitter;
    shared_ptr<Camera> camera;;
    float radius;
    float moveForce;
    float jumpForce;
    bool frozen;
    bool hasPowerUp;
    bool powerUpReady;
    std::vector<PowerUp *> storedPowerUp;

    float JUMP_TIME;
    int WANTS_JUMP;

    float LAND_TIME;
    int CAN_JUMP;
    vec3 LAST_NORMAL_FORCE;

    float JUMPED_AT_TIME;
    int JUST_JUMPED;
    vec3 startPosition;
    vec3 playPosition;
    int currentSkin = 0;
    vector<shared_ptr<Material>> marbleSkins;
};