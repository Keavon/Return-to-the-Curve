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
    map<string, shared_ptr<Skybox>> loadedSkyboxes;
    string relativePath;

public:
    SkyboxManager(string relativePath);
    shared_ptr<Skybox> get(string skyboxName, int textureUnit = 0, GLenum wrapX = GL_CLAMP_TO_EDGE, GLenum wrapY = GL_CLAMP_TO_EDGE);
    shared_ptr<Skybox> get(string skyboxName, vector<string> textureFile, int textureUnit = 0, GLenum wrapX = GL_CLAMP_TO_EDGE, GLenum wrapY = GL_CLAMP_TO_EDGE);
};