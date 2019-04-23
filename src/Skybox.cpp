#include "Skybox.h"
#include "GLSL.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "stb_image.h"

using namespace std;

Skybox::Skybox() :
	filenames{""},
	tid(0)
{
	
}

Skybox::~Skybox()
{
	
}

void Skybox::init()
{
	glGenTextures(1, &tid);
	glBindTexture(GL_TEXTURE_CUBE_MAP, tid);
	stbi_set_flip_vertically_on_load(false);
	int w, h, ncomps;
    for (int i = 0; i < 6; i++)
    {
	    unsigned char *data = stbi_load(filenames[i].c_str(), &w, &h, &ncomps, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        }
        else
        {
		    cerr << filenames[i] << " not found" << endl;
        }
    	stbi_image_free(data);
    }

	
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void Skybox::setWrapModes(GLint wrapS, GLint wrapT)
{
	// Must be called after init()
	glBindTexture(GL_TEXTURE_CUBE_MAP, tid);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
}

void Skybox::bind(GLint handle)
{
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_CUBE_MAP, tid);
	glUniform1i(handle, unit);
}

void Skybox::unbind()
{
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void Skybox::setFilenames(string f[6])
{
    for (int i = 0; i < 6; i++)
    {
        filenames[i] = f[i];
    }
}