#include "SceneManager.h"

SceneManager::SceneManager(shared_ptr<PrefabManager> prefabManager) : octree(vec3(-100), vec3(100))
{
    this->prefabManager = prefabManager;
}

void SceneManager::load(string sceneFile)
{
    YAML::Node file = YAML::LoadFile(sceneFile);

    printf("Loading Scene: %s\n", file["name"].as<string>().c_str());

    YAML::Node lightNode = file["light"];
    light.direction = ARRAY_TO_VEC3(lightNode["direction"]);
    light.brightness = ARRAY_TO_VEC3(lightNode["color"]) * lightNode["intensity"].as<float>();

    // Marble
    YAML::Node marble = file["marble"];
    marbleStart = ARRAY_TO_VEC3(marble["location"]);
    deathBelow = marble["deathBelow"].as<float>();

    shared_ptr<Instance> instance = prefabManager->get(marble["prefab"].as<string>())->getNewInstance();
    YAML::Node location = marble["location"];
    if (location) instance->physicsObject->position = ARRAY_TO_VEC3(location);

    YAML::Node rotation = marble["rotation"];
    if (rotation) instance->physicsObject->orientation = quat(ARRAY_TO_VEC3(rotation));

    YAML::Node scale = marble["scale"];
    if (scale) instance->physicsObject->scale = ARRAY_TO_VEC3(scale);

    scene.push_back(instance);
    octree.insert(instance->physicsObject);

    for (YAML::Node object : file["track"])
    {
        shared_ptr<Instance> instance = prefabManager->get(object["prefab"].as<string>())->getNewInstance();

        YAML::Node location = object["location"];
        if (location) instance->physicsObject->position = ARRAY_TO_VEC3(location);

        YAML::Node rotation = object["rotation"];
        if (rotation) instance->physicsObject->orientation = quat(ARRAY_TO_VEC3(rotation));

        YAML::Node scale = object["scale"];
        if (scale) instance->physicsObject->scale = ARRAY_TO_VEC3(scale);

        scene.push_back(instance);
        octree.insert(instance->physicsObject);
    }
    
    octree.fitToObjects();
}

shared_ptr<Instance> SceneManager::findInstance(string name)
{
    for (shared_ptr<Instance> current : scene)
    {
        if (current->definition.name == name) return current;
    }

    return nullptr;
}