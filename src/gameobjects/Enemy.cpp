#include "Enemy.h"

#include "../Shape.h"
#include "../WindowManager.h"
#include "../engine/ColliderSphere.h"
#include "../engine/PhysicsObject.h"

#include <glm/glm.hpp>
#include <memory>
#include <cmath>

using namespace glm;
using namespace std;

Enemy::Enemy(std::vector<glm::vec3> enemyPath, quat orientation, shared_ptr<Shape> model, float radius):
    PhysicsObject(position, orientation, model, make_shared<ColliderSphere>(radius)),
    radius(radius)
{
    pathCtrlPts = enemyPath;
    position = pathCtrlPts[0];
    speed = 0;
    material = 0;

    moveSpeed = 8;
    t = 0.1;
    acceleration = vec3(0, 0, 0);
    direction = vec3(0);
    
    forward = true;
    pointReached = true;
}

void Enemy::init(WindowManager *windowManager)
{
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
        targetY = 
                pow(1 - t, 3)*pathCtrlPts[0].y +
                3*t*pow(1-t,2)*pathCtrlPts[1].y +
                3*pow(t,2)*(1-t)*pathCtrlPts[2].y +
                pow(t,3)*pathCtrlPts[3][1];
        targetZ = 
                pow(1 - t, 3)*pathCtrlPts[0].z +
                3*t*pow(1-t,2)*pathCtrlPts[1].z +
                3*pow(t,2)*(1-t)*pathCtrlPts[2].z +
                pow(t,3)*pathCtrlPts[3][2];

        if (forward) {
            t += 0.02;
            //printf("Incremented t to : %f\n", t);
        }
        else {
            t -= 0.02;
            //printf("Decremented t to : %f\n", t);
        }
        if (t < 0){
            forward = true;
        }
        if (t > 1) {
            forward = false;
        }
        pointReached = false;
    }
        float dX = targetX - position.x;
        float dZ = targetZ - position.z;
        float dY = targetY - position.y;
        direction = normalize(vec3{dX ,dY ,dZ});
        velocity.x = velocity.z = 0;
        velocity.y = 0;
        //vec3 axis = vec3{0,1,0};
        //quat q = rotate(, axis);
        //orientation = q * orientation;
        velocity = direction * moveSpeed*dt;
        //printf("Velocity: %f, %f, %f", velocity.x,velocity.y,velocity.z);
        position += velocity;
        //printf("Position of Enemy: (%f,%f,%f)\n", position.x,position.y,position.z);
        if (sqrt( pow((targetX - position.x), 2) + 
                pow((targetZ - position.z), 2)) 
            < 1 ) {
            pointReached = true;
        }
}