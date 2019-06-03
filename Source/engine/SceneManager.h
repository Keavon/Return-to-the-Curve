#pragma once

#include <string>
#include <glm/glm.hpp>

class SceneManager
{
public:
    void load(std::string sceneFile);
    void save(std::string sceneFile);
};