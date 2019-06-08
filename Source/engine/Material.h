#pragma once

#include <string>
#include <glm/glm.hpp>

#include "Texture.h"
#include "TextureManager.h"
#include "Program.h"

class Material
{
private:
    std::shared_ptr<TextureManager> textureManager;
    std::shared_ptr<Program> pbr;

public:
    std::shared_ptr<Texture> albedo;
    std::shared_ptr<Texture> roughness;
    std::shared_ptr<Texture> metallic;
    std::shared_ptr<Texture> ao;

    Material(std::shared_ptr<Program> pbr, std::shared_ptr<TextureManager> textureManager);
    Material(std::shared_ptr<Program> pbr, std::shared_ptr<TextureManager> textureManager, std::string materialName, std::string extension);

    void load(std::string materialName, std::string extension);
    void bind();
};