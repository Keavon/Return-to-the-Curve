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

Enemy::Enemy(vec3 position, quat orientation, shared_ptr<Shape> model, float radius) :
    PhysicsObject(position, orientation, model, make_shared<ColliderSphere>(this, radius)),
    radius(radius)
{
    pathCtrlPts = {
        vec3{-10.0, 0.0, 2.0},
        vec3{-5.0, 0.0, 15.0},
        vec3{5.0, 0.0, 15.0},
        vec3{10, 0.0, 2.0}
    };
    position = pathCtrlPts[0];
    position.y = 1;
    speed = 0;
    material = 0;

    moveSpeed = 5;
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
        /*
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
        */
       targetX = t*10.0;
       targetZ = t*2.0;
        if (forward){
            t += 0.05;
            printf("Incremented t to : %f", t);
        }
        else {
            t -= 0.05;
            printf("Decremented t to : %f", t);
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
        printf("Direction: (%f,%f,%f)\n", direction.x,direction.z);
        velocity.x = velocity.z = 0;
        velocity.y = 0;
        //vec3 axis = vec3{0,1,0};
        //quat q = rotate(, axis);
        //orientation = q * orientation;
        velocity = direction * moveSpeed*dt;
        printf("Velocity: %f, %f, %f", velocity.x,velocity.y,velocity.z);
        position += velocity;
        //printf("Position of Enemy: (%d,%d,%d)\n", position.x,position.y,position.z);
        printf("Dt: %f\n", dt);
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
        printf("Point Reached: %s", pointReached ? "true\n" : "false\n");
}