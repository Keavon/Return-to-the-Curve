#pragma once

#include <string>
#include <vector>
#include <map>
#include <glm/glm.hpp>
#include <memory>

#include "Texture.h"

using namespace std;
using namespace glm;

#define CONTAINS(map, key) map.find(key) != map.end()

class TextureManager
{
private:
    std::map<std::string, std::shared_ptr<Texture>> loadedTextures;
    std::string relativePath;

public:
    TextureManager(std::string relativePath);
    std::shared_ptr<Texture> get(std::string textureFile, int textureUnit = 0, GLenum wrapX = GL_REPEAT, GLenum wrapY = GL_REPEAT);
};