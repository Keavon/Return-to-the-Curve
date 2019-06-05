#pragma once

#include <string>
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <yaml-cpp/yaml.h>
#include <algorithm>

#include "PhysicsObject.h"
#include "Material.h"
#include "Prefab.h"
#include "Instance.h"
#include "PrefabManager.h"
#include "Octree.h"

class SceneManager
{
private:
    std::shared_ptr<PrefabManager> prefabManager;

public:
    std::vector<std::shared_ptr<Instance>> scene;
    Octree octree;
    glm::vec3 marbleStart;
    float deathBelow;

    SceneManager(std::shared_ptr<PrefabManager> prefabManager);
    void load(std::string sceneFile);
    void save(std::string sceneFile);
};
