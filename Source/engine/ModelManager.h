#pragma once

#include <string>
#include <map>
#include <glm/glm.hpp>

#include "Shape.h"

class ModelManager
{
private:
    std::map<std::string, std::shared_ptr<Shape>> loadedModels;
    std::string relativePath;

public:
    ModelManager(std::string relativePath);
    std::shared_ptr<Shape> get(std::string modelFile, bool resize = false, bool findEdges = false);
};