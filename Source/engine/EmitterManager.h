#pragma once

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <glm/glm.hpp>

#include "ParticleEmitter.h"

class EmitterManager
{
private:
    std::map<std::string, std::shared_ptr<ParticleEmitter>> loadedEmitters;

public:
    std::shared_ptr<ParticleEmitter> get(std::string name);
    std::shared_ptr<ParticleEmitter> get(std::string name, std::shared_ptr<Shape> billboard, std::shared_ptr<Texture> texture, int maxParticles);

    std::vector<std::shared_ptr<ParticleEmitter>> list();
    std::vector<std::string> listNames();
};