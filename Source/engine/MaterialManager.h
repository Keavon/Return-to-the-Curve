#pragma once

#include <string>
#include <map>
#include <glm/glm.hpp>

#include "Material.h"

class MaterialManager
{
private:
    std::map<std::string, std::shared_ptr<Material>> loadedMaterials;
    std::shared_ptr<Program> pbr;
    std::shared_ptr<TextureManager> textureManager;

public:
    void init(std::shared_ptr<Program> pbr, std::shared_ptr<TextureManager> textureManager);
    std::shared_ptr<Material> get(std::string materialName, std::string extension);
};