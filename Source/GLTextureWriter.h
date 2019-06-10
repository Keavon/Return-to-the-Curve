#pragma once

#include <string>
#include <memory>
#include "Texture.h"
#include <GLFW/glfw3.h>
#include <iostream>

namespace GLTextureWriter
{
	using namespace std;
	
	bool WriteImage(shared_ptr<Texture> texture, string fileName);
	bool WriteImage(const Texture & texture, string fileName);
	bool WriteImage(GLint textureHandle, string fileName);
	bool WriteDepthImage(GLint tid, string imgName);
}
