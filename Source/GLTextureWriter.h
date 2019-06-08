#pragma once

#include <string>
#include <memory>
#include "Texture.h"
#include <GLFW/glfw3.h>
#include <iostream>

namespace GLTextureWriter
{
	bool WriteImage(std::shared_ptr<Texture> texture, std::string fileName);
	bool WriteImage(const Texture & texture, std::string fileName);
	bool WriteImage(GLint textureHandle, std::string fileName);
}
