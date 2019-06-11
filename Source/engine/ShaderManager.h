#pragma once

#include <string>
#include <map>
#include <iostream>
#include <glm/glm.hpp>
#include <memory>

#include "Program.h"

using namespace std;
using namespace glm;

#define CONTAINS(map, key) map.find(key) != map.end()

class ShaderManager
{
private:
    map<string, shared_ptr<Program>> loadedPrograms;
    string relativePath;

public:
    ShaderManager(string relativePath);
    shared_ptr<Program> get(string programFile);
    shared_ptr<Program> get(string programFile, vector<string> attributes, vector<string> uniforms);
};