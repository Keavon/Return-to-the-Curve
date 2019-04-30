#include "Enemy.h"

#include "../Shape.h"
#include "../WindowManager.h"
#include "../engine/ColliderSphere.h"

#include <glm/glm.hpp>
#include <memory>
#include <cmath>

using namespace glm;
using namespace std;

Enemy::Enemy(float radius) :
    radius(radius)
{
    pathCtrlPts = {
        vec3{1.0, 0.0, 0.0},
        vec3{1.0, 0.0, -0.5},
        vec3{1.0, 0.0, 0.0},
        vec3{-1.0, 0.0, 0.5}
    };
    position = pathCtrlPts[0];
    position.y = 1;
    collider = ColliderSphere(radius);
    speed = 0;
    material = 0;
    orientation = quat(1, 0, 0, 0);

    moveSpeed = 5;
    acceleration = vec3(0, -20, 0);
    direction = vec3(0);
    
    forward = true;
    pointReached = true;
}

void Enemy::init(shared_ptr<Shape> model, WindowManager *windowManager)
{
    this->model = model;
    this->windowManager = windowManager;
}

void Enemy::update(float dt)
{
    if (pointReached) {
        //Calculate new position over factor t
        targetX = 
                pow(1 - t, 3)*pathCtrlPts[0].x +
                3*t*pow(1-t,2)*pathCtrlPts[1].x +
                3*pow(t,2)*(1-t)*pathCtrlPts[2].x +
                pow(t,3)*pathCtrlPts[3][0];
        targetZ = 
                pow(1 - t, 3)*pathCtrlPts[0].z +
                3*t*pow(1-t,2)*pathCtrlPts[1].z +
                3*pow(t,2)*(1-t)*pathCtrlPts[2].z +
                pow(t,3)*pathCtrlPts[3][2];
        if (forward) {
            t += 0.05;
        }
        else {
            t -= 0.05;
        }
        if (t < 0){
            forward = true;
        }
        if (t > 1) {
            forward = false;
        }
        pointReached = false;
        printf("New X: %f\nNew Z: %f\nt = %f", targetX, targetZ, t);
    }
    float dX = targetX - position.x;
    float dZ = targetZ - position.z;
    direction = normalize(vec3(dX,0,dZ));

    velocity.x = velocity.z = 0;
    //vec3 axis = vec3{0,1,0};
    //quat q = rotate(, axis);
    //orientation = q * orientation;
    velocity += direction * moveSpeed;
    position += velocity * dt;

    // Keeps Enemy on the plane.
    if (position.y < radius)
    {
        velocity.y = 0;
        position.y = radius;
    }

    if (sqrt( pow((targetX - position.x), 2) + 
              pow((targetZ - position.z), 2)) 
        < 1 ) {
        pointReached = true;
    }
}