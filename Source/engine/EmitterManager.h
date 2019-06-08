#pragma once

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <glm/glm.hpp>

#include "ParticleEmitter.h"

using namespace std;
using namespace glm;

#define CONTAINS(map, key) map.find(key) != map.end()

class EmitterManager
{
private:
    map<string, shared_ptr<ParticleEmitter>> loadedEmitters;

public:
    shared_ptr<ParticleEmitter> get(string name);
    shared_ptr<ParticleEmitter> get(string name, shared_ptr<Shape> billboard, shared_ptr<Texture> texture, int maxParticles);

    vector<shared_ptr<ParticleEmitter>> list();
    vector<string> listNames();
};