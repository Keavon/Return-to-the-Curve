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
    map<string, shared_ptr<Texture>> loadedTextures;
    string relativePath;

public:
    TextureManager(string relativePath);
    shared_ptr<Texture> get(string textureFile, int textureUnit = 0, GLenum wrapX = GL_REPEAT, GLenum wrapY = GL_REPEAT);
};