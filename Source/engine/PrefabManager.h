#pragma once

#include <string>
#include <map>
#include <glm/glm.hpp>
#include <yaml-cpp/yaml.h>

#include "MaterialManager.h"
#include "ModelManager.h"
#include "Shape.h"
#include "Prefab.h"

using namespace std;
using namespace glm;

#define CONTAINS(map, key) map.find(key) != map.end()

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