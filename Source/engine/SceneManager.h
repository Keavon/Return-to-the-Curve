#pragma once

#include <string>
#include <glm/glm.hpp>
#include <vector>
#include <memory>

#include "PhysicsObject.h"
#include "Material.h"
#include "Prefab.h"
#include "Instance.h"

class SceneManager
{
public:
    std::vector<std::shared_ptr<Instance>> scene;

    void load(std::string sceneFile);
    void save(std::string sceneFile);
};
