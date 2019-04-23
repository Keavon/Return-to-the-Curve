#pragma once

#include "Collider.h"

class ColliderSphere : public Collider
{
public:
    ColliderSphere();
    ColliderSphere(float radius);
    float radius;
};