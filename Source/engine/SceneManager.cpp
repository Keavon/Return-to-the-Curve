#include "SceneManager.h"

using namespace std;
using namespace glm;

#define ARRAY_TO_VEC3(arrayOf3) vec3(arrayOf3[0].as<float>(), arrayOf3[1].as<float>(), arrayOf3[2].as<float>())

SceneManager::SceneManager(shared_ptr<PrefabManager> prefabManager)
{
    this->prefabManager = prefabManager;

    this->octree = Octree(vec3(-200, -210, -200), vec3(200, 190, 200));
    //octree->init(modelManager->get("billboard.obj"), modelManager->get("cube.obj"));
}

void SceneManager::load(string sceneFile)
{
    YAML::Node file = YAML::LoadFile(sceneFile);

    printf("Loading Scene: %s\n", file["name"].as<string>().c_str());

    vec3 globalScale = ARRAY_TO_VEC3(file["globalScale"]);

    marbleStart = ARRAY_TO_VEC3(file["marble"]["location"]) * globalScale;

    deathBelow = file["marble"]["deathBelow"].as<float>();

    for (YAML::Node object : file["track"])
    {
        shared_ptr<Instance> instance = prefabManager->get(object["prefab"].as<string>())->getNewInstance();

        YAML::Node location = object["location"];
        if (location) instance->physicsObject->position = ARRAY_TO_VEC3(location);
        instance->physicsObject->position *= globalScale;

        YAML::Node rotation = object["rotation"];
        if (rotation) instance->physicsObject->orientation = quat(ARRAY_TO_VEC3(rotation));

        YAML::Node scale = object["scale"];
        if (scale) instance->physicsObject->scale = ARRAY_TO_VEC3(scale);
        instance->physicsObject->scale *= globalScale;

        scene.push_back(instance);
        octree.insert(instance->physicsObject);
    }
}