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

using namespace std;

string readFileAsString(const string &fileName);

class Program
{

public:

	void setVerbose(const bool v) { verbose = v; }
	bool isVerbose() const { return verbose; }

	void setShaderNames(const string &v, const string &f);
	virtual bool init();
	virtual void bind();
	virtual void unbind();

	void addAttribute(const string &name);
	void addUniform(const string &name);
	bool hasAttribute(string attribute);
	bool hasUniform(string uniform);
	GLint getAttribute(const string &name) const;
	GLint getUniform(const string &name) const;

protected:

	string vShaderName;
	string fShaderName;

private:

	GLuint pid = 0;
	map<string, GLint> attributes;
	map<string, GLint> uniforms;
	bool verbose = true;

};
