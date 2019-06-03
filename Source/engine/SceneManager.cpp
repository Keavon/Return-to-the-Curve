#include "SceneManager.h"

#include <glm/glm.hpp>
#include <yaml-cpp/yaml.h>
#include <algorithm>

using namespace std;
using namespace glm;

void SceneManager::load(string sceneFile)
{
    YAML::Node file = YAML::LoadFile(sceneFile);
    printf("Scene Name: %s\n", file["name"].as<string>().c_str());

    //vec3 transform;
    //while (inLevel >> transform.x) {
    //    inLevel >> transform.y >> transform.z;
    //    auto box = make_shared<Box>(vec3(transform.x * 8, transform.y, transform.z * 6), normalize(quat(0, 0, 0, 0)), shapes.boxModel);
    //    boxes.push_back(box);
    //}
    //octree->insert(boxes);
}