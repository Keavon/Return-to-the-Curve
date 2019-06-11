#include "PrefabManager.h"

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
        
        shared_ptr<Prefab> prefab = make_shared<Prefab>(prefabFile, model, material);
        loadedPrefabs[prefabFile] = prefab;

        YAML::Node physics = file["physics"];
        if (physics) {
            if (physics["mass"]) prefab->mass = physics["mass"].as<float>();
            if (physics["friction"]) prefab->friction = physics["friction"].as<float>();
            if (physics["elasticity"]) prefab->elasticity = physics["elasticity"].as<float>();
        }
    }
    
    return loadedPrefabs[prefabFile];
}