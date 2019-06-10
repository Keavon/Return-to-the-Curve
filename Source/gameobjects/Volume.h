#pragma once

#include "../engine/PhysicsObject.h"
#include "../engine/TriggerSphere.h"
#include "../Shape.h"

#include <glm/glm.hpp>
#include <memory>
#include <iostream>

class Volume : public PhysicsObject
{
public:
    Volume(glm::vec3 position, glm::quat orientation, std::shared_ptr<Shape> model);
};