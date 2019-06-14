#include "Material.h"

Material::Material(shared_ptr<Program> pbr, shared_ptr<TextureManager> textureManager, shared_ptr<SkyboxManager> skyboxManager)
{
    this->pbr = pbr;
    this->textureManager = textureManager;
    this->skyboxManager = skyboxManager;
}

Material::Material(shared_ptr<Program> pbr, shared_ptr<TextureManager> textureManager, shared_ptr<SkyboxManager> skyboxManager, string materialName, string extension) {
    this->pbr = pbr;
    this->textureManager = textureManager;
    this->skyboxManager = skyboxManager;
    load(materialName, extension);
}

void Material::load(string materialName, string extension)
{
    albedo = textureManager->get("materials/" + materialName + "/albedo." + extension, 2);
    roughness = textureManager->get("materials/" + materialName + "/roughness." + extension, 3);
    metallic = textureManager->get("materials/" + materialName + "/metallic." + extension, 4);
    ao = textureManager->get("materials/" + materialName + "/ao." + extension, 5);
}

void Material::bind() {
    albedo->bind(pbr->getUniform("albedoMap"));
    roughness->bind(pbr->getUniform("roughnessMap"));
    metallic->bind(pbr->getUniform("metallicMap"));
    ao->bind(pbr->getUniform("aoMap"));
    skyboxManager->getCurrent()->bind(pbr->getUniform("skybox"));
}
