#include "Material.h"

Material::Material(shared_ptr<Program> pbr, shared_ptr<TextureManager> textureManager)
{
    this->pbr = pbr;
    this->textureManager = textureManager;
}

Material::Material(shared_ptr<Program> pbr, shared_ptr<TextureManager> textureManager, string materialName, string extension) {
    this->pbr = pbr;
    this->textureManager = textureManager;
    load(materialName, extension);
}

void Material::load(string materialName, string extension)
{
    albedo = textureManager->get("materials/" + materialName + "/albedo." + extension, 1);
    roughness = textureManager->get("materials/" + materialName + "/roughness." + extension, 2);
    metallic = textureManager->get("materials/" + materialName + "/metallic." + extension, 3);
    ao = textureManager->get("materials/" + materialName + "/ao." + extension, 4);
}

void Material::bind() {
    albedo->bind(pbr->getUniform("albedoMap"));
    roughness->bind(pbr->getUniform("roughnessMap"));
    metallic->bind(pbr->getUniform("metallicMap"));
    ao->bind(pbr->getUniform("aoMap"));
}
