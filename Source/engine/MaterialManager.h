#pragma once

#include <string>
#include <map>
#include <glm/glm.hpp>

#include "Material.h"

using namespace std;
using namespace glm;

#define CONTAINS(map, key) map.find(key) != map.end()

class MaterialManager
{
private:
    map<string, shared_ptr<Material>> loadedMaterials;
    shared_ptr<Program> pbr;
    shared_ptr<TextureManager> textureManager;

public:
    void init(shared_ptr<Program> pbr, shared_ptr<TextureManager> textureManager);
    shared_ptr<Material> get(string materialName, string extension);
};