#pragma once

#include <glad/glad.h>
#include <string>

class Skybox
{
public:
	Skybox();
	virtual ~Skybox();
	void setFilenames(std::string f[6]);
	void init();
	void setUnit(GLint u) { unit = u; }
	GLint getUnit() const { return unit; }
	void bind(GLint handle);
	void unbind();
	void setWrapModes(GLint wrapS, GLint wrapT); // Must be called after init()
	GLint getID() const { return tid;}
private:
	std::string filenames[6];
	int width;
	int height;
	GLuint tid;
	GLint unit;
	
};