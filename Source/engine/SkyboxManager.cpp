#include "SkyboxManager.h"

SkyboxManager::SkyboxManager(string relativePath) {
    this->relativePath = relativePath;
}

shared_ptr<Skybox> loadTexture(string path, vector<string> files, int textureUnit, GLenum wrapX, GLenum wrapY) {
    shared_ptr<Skybox> skybox = make_shared<Skybox>();
    vector<string> textures = { path + files[0], path + files[1], path + files[2], path + files[3], path + files[4], path + files[5] };
    skybox->setFilenames(textures.data());
    skybox->init();
    skybox->setUnit(textureUnit);
    skybox->setWrapModes(wrapX, wrapY);
    return skybox;
}

shared_ptr<Skybox> SkyboxManager::get(string skyboxName, int textureUnit, GLenum wrapX, GLenum wrapY)
{
    return SkyboxManager::get(skyboxName, { "px.jpg", "nx.jpg", "py.jpg", "ny.jpg", "pz.jpg", "nz.jpg" }, textureUnit, wrapX, wrapY);
}

shared_ptr<Skybox> SkyboxManager::get(string skyboxName, vector<string> skyboxFiles, int textureUnit, GLenum wrapX, GLenum wrapY)
{
    if (!(CONTAINS(loadedSkyboxes, skyboxName))) {
        loadedSkyboxes[skyboxName] = loadTexture(relativePath + skyboxName + "/", skyboxFiles, textureUnit, wrapX, wrapY);
    }

    return loadedSkyboxes[skyboxName];
}
