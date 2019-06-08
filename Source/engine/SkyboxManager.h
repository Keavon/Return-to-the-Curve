#pragma once

#include <string>
#include <vector>
#include <map>
#include <glm/glm.hpp>
#include <memory>

#include "Skybox.h"

using namespace std;
using namespace glm;

#define CONTAINS(map, key) map.find(key) != map.end()

class SkyboxManager
{
private:
    std::map<std::string, std::shared_ptr<Skybox>> loadedSkyboxes;
    std::string relativePath;

public:
    SkyboxManager(std::string relativePath);
    std::shared_ptr<Skybox> get(std::string skyboxName, int textureUnit = 0, GLenum wrapX = GL_CLAMP_TO_EDGE, GLenum wrapY = GL_CLAMP_TO_EDGE);
    std::shared_ptr<Skybox> get(std::string skyboxName, std::vector<std::string> textureFile, int textureUnit = 0, GLenum wrapX = GL_CLAMP_TO_EDGE, GLenum wrapY = GL_CLAMP_TO_EDGE);
};