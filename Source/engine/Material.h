#pragma once

#include <string>
#include <glm/glm.hpp>

#include "Texture.h"
#include "TextureManager.h"
#include "Program.h"
#include "SkyboxManager.h"

using namespace std;
using namespace glm;

class Material
{
private:
    shared_ptr<TextureManager> textureManager;
    shared_ptr<Program> pbr;
    shared_ptr<SkyboxManager> skyboxManager;

public:
    shared_ptr<Texture> albedo;
    shared_ptr<Texture> roughness;
    shared_ptr<Texture> metallic;
    shared_ptr<Texture> ao;

    Material(shared_ptr<Program> pbr, shared_ptr<TextureManager> textureManager, shared_ptr<SkyboxManager> skyboxManager);
    Material(shared_ptr<Program> pbr, shared_ptr<TextureManager> textureManager, shared_ptr<SkyboxManager> skyboxManager, string materialName, string extension);

    void load(string materialName, string extension);
    void bind();
};