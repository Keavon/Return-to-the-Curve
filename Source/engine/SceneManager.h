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
#include "MaterialManager.h"
#include "Octree.h"
#include "../gameobjects/Enemy.h"

using namespace std;
using namespace glm;

#define ARRAY_TO_VEC3(arrayOf3) vec3(arrayOf3[0].as<float>(), arrayOf3[1].as<float>(), arrayOf3[2].as<float>())

class SceneManager
{
private:
    shared_ptr<PrefabManager> prefabManager;
    shared_ptr<MaterialManager> materialManager;

public:
    vector<shared_ptr<Instance>> scene;
    Octree octree;
    vec3 marbleStart;
    float deathBelow;

    struct {
        vec3 direction;
        vec3 brightness;
    } light;

    SceneManager(shared_ptr<PrefabManager> prefabManager, shared_ptr<MaterialManager> materialManager);
    shared_ptr<Instance> findInstance(string name);
    void load(string sceneFile);
    void save(string sceneFile);
};
