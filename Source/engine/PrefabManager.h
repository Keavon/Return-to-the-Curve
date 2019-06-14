#pragma once

#include <string>
#include <map>
#include <glm/glm.hpp>
#include <yaml-cpp/yaml.h>
#include "stdio.h"

#include "MaterialManager.h"
#include "ModelManager.h"
#include "Shape.h"
#include "Prefab.h"

using namespace std;
using namespace glm;

#define CONTAINS(map, key) map.find(key) != map.end()
#define ARRAY_TO_VEC3(arrayOf3) vec3(arrayOf3[0].as<float>(), arrayOf3[1].as<float>(), arrayOf3[2].as<float>())

class PrefabManager
{
public:
    shared_ptr<MaterialManager> materialManager;
    shared_ptr<ModelManager> modelManager;
    map<string, shared_ptr<Prefab>> loadedPrefabs;
    string relativePath;

    PrefabManager(string relativePath, shared_ptr<ModelManager> modelManager, shared_ptr<MaterialManager> materialManager);

    shared_ptr<Prefab> get(string prefabFile);
};