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
    map<string, shared_ptr<Shape>> loadedModels;
    string relativePath;

public:
    ModelManager(string relativePath);
    shared_ptr<Shape> get(string modelFile, bool resize = false, bool findEdges = false);
};