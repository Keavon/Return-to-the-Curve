#pragma once

#include <string>
#include <map>
#include <glm/glm.hpp>

#include "Material.h"
#include "SkyboxManager.h"

using namespace std;
using namespace glm;

#define CONTAINS(map, key) map.find(key) != map.end()

class MaterialManager
{
private:
    map<string, shared_ptr<Material>> loadedMaterials;
    shared_ptr<Program> pbr;
    shared_ptr<TextureManager> textureManager;
    shared_ptr<SkyboxManager> skyboxManager;

public:
    MaterialManager(shared_ptr<SkyboxManager> skyboxManager);
    void init(shared_ptr<Program> pbr, shared_ptr<TextureManager> textureManager);
    shared_ptr<Material> get(string materialName, string extension);
};