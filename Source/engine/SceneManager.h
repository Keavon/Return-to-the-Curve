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

using namespace std;
using namespace glm;

#define ARRAY_TO_VEC3(arrayOf3) vec3(arrayOf3[0].as<float>(), arrayOf3[1].as<float>(), arrayOf3[2].as<float>())

class SceneManager
{
private:
    std::shared_ptr<PrefabManager> prefabManager;

public:
    std::vector<std::shared_ptr<Instance>> scene;
    Octree octree;
    glm::vec3 marbleStart;
    float deathBelow;

    struct {
        glm::vec3 direction;
        glm::vec3 brightness;
    } light;

    SceneManager(std::shared_ptr<PrefabManager> prefabManager);
    void load(std::string sceneFile);
    void save(std::string sceneFile);
};
