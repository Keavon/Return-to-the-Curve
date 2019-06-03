#pragma once

#include <string>
#include <vector>
#include <map>
#include <glm/glm.hpp>

#include "Texture.h"

class TextureManager
{
private:
    std::map<std::string, std::shared_ptr<Texture>> loadedTextures;
    std::string relativePath;

public:
    TextureManager(std::string relativePath);
    std::shared_ptr<Texture> get(std::string textureFile, int textureUnit = 0, GLenum wrapX = GL_REPEAT, GLenum wrapY = GL_REPEAT);
};