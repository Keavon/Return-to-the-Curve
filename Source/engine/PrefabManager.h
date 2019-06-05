#pragma once

#include <string>
#include <map>
#include <glm/glm.hpp>
#include <yaml-cpp/yaml.h>

#include "MaterialManager.h"
#include "ModelManager.h"
#include "Shape.h"
#include "Prefab.h"

class PrefabManager
{
public:
    std::shared_ptr<MaterialManager> materialManager;
    std::shared_ptr<ModelManager> modelManager;
    std::map<std::string, std::shared_ptr<Prefab>> loadedPrefabs;
    std::string relativePath;

    PrefabManager(std::string relativePath, std::shared_ptr<ModelManager> modelManager, std::shared_ptr<MaterialManager> materialManager);

    std::shared_ptr<Prefab> get(std::string prefabFile);
};