#pragma  once

#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <cassert>
#include <fstream>
#include <glad/glad.h>

#include "GLSL.h"

#define CONTAINS(map, key) map.find(key) != map.end()

std::string readFileAsString(const std::string &fileName);

class Program
{

public:

	void setVerbose(const bool v) { verbose = v; }
	bool isVerbose() const { return verbose; }

	void setShaderNames(const std::string &v, const std::string &f);
	virtual bool init();
	virtual void bind();
	virtual void unbind();

	void addAttribute(const std::string &name);
	void addUniform(const std::string &name);
	bool hasAttribute(std::string attribute);
	bool hasUniform(std::string uniform);
	GLint getAttribute(const std::string &name) const;
	GLint getUniform(const std::string &name) const;

protected:

	std::string vShaderName;
	std::string fShaderName;

private:

	GLuint pid = 0;
	std::map<std::string, GLint> attributes;
	std::map<std::string, GLint> uniforms;
	bool verbose = true;

};
