#pragma once

#include <string>
#include <map>
#include <iostream>
#include <glm/glm.hpp>
#include <memory>

#include "Program.h"

class ShaderManager
{
private:
    std::map<std::string, std::shared_ptr<Program>> loadedPrograms;
    std::string relativePath;

public:
    ShaderManager(std::string relativePath);
    std::shared_ptr<Program> get(std::string programFile);
    std::shared_ptr<Program> get(std::string programFile, std::vector<std::string> attributes, std::vector<std::string> uniforms);
};