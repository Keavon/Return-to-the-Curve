#pragma once

#include <string>
#include <glm/glm.hpp>

#include "Texture.h"
#include "TextureManager.h"
#include "Program.h"

using namespace std;
using namespace glm;

class Material
{
private:
    shared_ptr<TextureManager> textureManager;
    shared_ptr<Program> pbr;

public:
    shared_ptr<Texture> albedo;
    shared_ptr<Texture> roughness;
    shared_ptr<Texture> metallic;
    shared_ptr<Texture> ao;

    Material(shared_ptr<Program> pbr, shared_ptr<TextureManager> textureManager);
    Material(shared_ptr<Program> pbr, shared_ptr<TextureManager> textureManager, string materialName, string extension);

    void load(string materialName, string extension);
    void bind();
};