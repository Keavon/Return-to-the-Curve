#pragma once

#include <string>
#include <map>
#include <glm/glm.hpp>

#include "Shape.h"

using namespace std;
using namespace glm;

#define CONTAINS(map, key) map.find(key) != map.end()

class ModelManager
{
private:
    std::map<std::string, std::shared_ptr<Shape>> loadedModels;
    std::string relativePath;

public:
    ModelManager(std::string relativePath);
    std::shared_ptr<Shape> get(std::string modelFile, bool resize = false, bool findEdges = false);
};