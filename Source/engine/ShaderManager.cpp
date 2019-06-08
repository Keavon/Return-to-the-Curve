#include "ShaderManager.h"

ShaderManager::ShaderManager(string relativePath) {
    this->relativePath = relativePath;
}

shared_ptr<Program> initShader(string relativePath, string file, vector<string> attributes, vector<string> uniforms) {
    // Shader for textured models
    shared_ptr<Program> program = make_shared<Program>();
    program->setVerbose(true);
    program->setShaderNames(relativePath + file + ".vert.glsl", relativePath + file + ".frag.glsl");
    if (!program->init()) {
        cerr << "Failed to compile " << file << " shader" << endl;
        exit(1);
    }

    for (int i = 0; i < attributes.size(); i++) {
        program->addAttribute(attributes[i]);
    }

    for (int i = 0; i < uniforms.size(); i++) {
        program->addUniform(uniforms[i]);
    }

    return program;
}

shared_ptr<Program> ShaderManager::get(string programFile) {
    if (!(CONTAINS(loadedPrograms, programFile))) {
        cerr << "Shader program has not been loaded yet and it's being requested from the ShaderManager without the necessary parameters to load it now" << endl;
    }

    return loadedPrograms[programFile];
}

shared_ptr<Program> ShaderManager::get(string programFile, vector<string> attributes, vector<string> uniforms) {
    if (!(CONTAINS(loadedPrograms, programFile))) {
        loadedPrograms[programFile] = initShader(relativePath, programFile, attributes, uniforms);
    }

    return loadedPrograms[programFile];
}
