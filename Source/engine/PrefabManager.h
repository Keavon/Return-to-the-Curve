#pragma once

#include <string>
#include <map>
#include <glm/glm.hpp>
#include <yaml-cpp/yaml.h>

class Prefab {
public:
};

class PrefabManager
{
public:
    Prefab get(std::string prefabFile);

    std::map<std::string, Prefab> loadedPrefabs;
};