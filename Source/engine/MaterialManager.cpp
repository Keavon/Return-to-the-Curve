#include "MaterialManager.h"

MaterialManager::MaterialManager(shared_ptr<SkyboxManager> skyboxManager)
{
    this->skyboxManager = skyboxManager;
}

void MaterialManager::init(shared_ptr<Program> pbr, shared_ptr<TextureManager> textureManager) {
    this->pbr = pbr;
    this->textureManager = textureManager;
}

shared_ptr<Material> MaterialManager::get(string materialName, string extension)
{
    string materialIdentifier = materialName + "." + extension;

    if (!(CONTAINS(loadedMaterials, materialIdentifier))) {
        loadedMaterials[materialIdentifier] = make_shared<Material>(pbr, textureManager, skyboxManager, materialName, extension);
    }

    return loadedMaterials[materialIdentifier];
}
