#include "PrefabManager.h"

using namespace std;
using namespace glm;

#define CONTAINS(map, key) map.find(key) != map.end()

Prefab PrefabManager::get(string prefabFile)
{
    if (CONTAINS(loadedPrefabs, prefabFile)) {
        return loadedPrefabs[prefabFile];
    }
    else {
        YAML::Node file = YAML::LoadFile(prefabFile);
    }
}