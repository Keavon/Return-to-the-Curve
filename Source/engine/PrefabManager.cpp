#include "PrefabManager.h"

using namespace std;
using namespace glm;

#define CONTAINS(map, key) map.find(key) != map.end()

PrefabManager::PrefabManager(string relativePath, shared_ptr<ModelManager> modelManager, shared_ptr<MaterialManager> materialManager)
{
    this->relativePath = relativePath;
    this->modelManager = modelManager;
    this->materialManager = materialManager;
}

shared_ptr<Prefab> PrefabManager::get(string prefabFile)
{
    if (!(CONTAINS(loadedPrefabs, prefabFile))) {
        YAML::Node file = YAML::LoadFile(relativePath + prefabFile + ".yaml");

        shared_ptr<Shape> model = modelManager->get(file["model"].as<string>());
        shared_ptr<Material> material = materialManager->get(file["material"][0].as<string>(), file["material"][1].as<string>());
        
        shared_ptr<Prefab> prefab = make_shared<Prefab>(model, material);
        loadedPrefabs[prefabFile] = prefab;
    }
    
    return loadedPrefabs[prefabFile];
}