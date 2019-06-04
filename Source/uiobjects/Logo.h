#pragma once

#include "../engine/UIObject.h"
#include <glm/glm.hpp>
#include <memory>
#include "../Shape.h"

class Logo : public UIObject
{
public:
	Logo(glm::vec3 position, glm::quat orientation, std::shared_ptr<Shape> model);
};