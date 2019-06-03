#include "EmitterManager.h"

using namespace std;
using namespace glm;

#define CONTAINS(map, key) map.find(key) != map.end()

shared_ptr<ParticleEmitter> EmitterManager::get(string name) {
    if (!(CONTAINS(loadedEmitters, name))) {
        cerr << "Particle emitter has not been loaded yet and it's being requested from the EmitterManager without the necessary parameters to load it now" << endl;
    }

    return loadedEmitters[name];
}

shared_ptr<ParticleEmitter> EmitterManager::get(string name, shared_ptr<Shape> billboard, shared_ptr<Texture> texture, int maxParticles)
{
    if (!(CONTAINS(loadedEmitters, name))) {
        loadedEmitters[name] = make_shared<ParticleEmitter>(billboard, texture, maxParticles);
    }

    return loadedEmitters[name];
}

vector<shared_ptr<ParticleEmitter>> EmitterManager::list()
{
    vector<shared_ptr<ParticleEmitter>> list;
    list.reserve(loadedEmitters.size());
    for (pair<string, shared_ptr<ParticleEmitter>> const& mapEntryPair : loadedEmitters)
    {
        list.push_back(mapEntryPair.second);
    }
    return list;
}

vector<string> EmitterManager::listNames() {
    vector<string> list;
    list.reserve(loadedEmitters.size());
    for (pair<string, shared_ptr<ParticleEmitter>> const& mapEntryPair : loadedEmitters) {
        list.push_back(mapEntryPair.first);
    }
    return list;
}