#pragma once

#include <glad/glad.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "GLSL.h"

using namespace std;

class Texture
{
public:
	Texture();
	virtual ~Texture();
	void setFilename(const string &f) { filename = f; }
	void init();
	void setUnit(GLint u) { unit = u; }
	GLint getUnit() const { return unit; }
	void bind(GLint handle);
	void unbind();
	void setWrapModes(GLint wrapS, GLint wrapT); // Must be called after init()
	GLint getID() const { return tid;}
	
private:
	string filename;
	int width;
	int height;
	GLuint tid;
	GLint unit;
};
