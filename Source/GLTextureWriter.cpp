
#include "GLTextureWriter.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb-image/stb_image_write.h>

/**
 * Retrieve the width of the texture
 * @return the texture width, in pixels.
 */
int getTextureWidth()
{
	GLint texWidth;
	glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_WIDTH,&texWidth);
	return texWidth;
}

/**
 * Retrieve the height of the texture
 * @return the texture height, in pixels.
 */
int getTextureHeight()
{
	GLint texHeight;
	glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_HEIGHT,&texHeight);
	return texHeight;
}

/**
 * Retrieve data from a bound buffer
 */
void getData(void * dataBuffer, GLenum format, GLenum type)
{

	glGetTexImage(GL_TEXTURE_2D,
		0,
		format,
		type,
		dataBuffer);
	//Flip data buffer
}

/**
 * Write a texture to a file
 * @param  texture  a shared pointer to a texture
 * @param  fileName the name of the file
 * @return          true if successfull
 */
bool GLTextureWriter::WriteImage(shared_ptr<Texture> texture, string fileName)
{
	return WriteImage(texture->getID(), fileName);
}

/**
 * Write a texture to a file
 * @param  texture a texture object
 * @param  imgName the filename to write to
 * @return         true if successful.
 */
bool GLTextureWriter::WriteImage(const Texture &  texture, string imgName)
{
	return WriteImage(texture.getID(), imgName);
}

/**
 * Main logic of this code taken from stb_image.h
 * @param imgData Image data to flip
 * @param width   width of the image
 * @param height  height of the image
 */
void flip_buffer(char * imgData, int width, int height )
{

	int w =width, h = height;
	int depth = 3;
	int row,col,z;
	char temp;


	for (row = 0; row < (h>>1); row++) {
		for (col = 0; col < w; col++) {
			for (z = 0; z < depth; z++) {
				temp = imgData[(row * w + col) * depth + z];
				imgData[(row * w + col) * depth + z] = imgData[((h - row - 1) * w + col) * depth + z];
				imgData[((h - row - 1) * w + col) * depth + z] = temp;
			}
		}
	}
}

bool GLTextureWriter::WriteImage(GLint tid, string imgName)
{
	//Backup old openGL state.
	GLint backupBoundTexture;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &backupBoundTexture);

	//Bind texture to buffer
	glBindTexture(GL_TEXTURE_2D,tid);

	//Retrieve width and height
	int txWidth = getTextureWidth();
	int txHeight = getTextureWidth();


	//Allocate buffer
	float * dataBuffer = new float[txWidth*txHeight*4];

	//Get data from Opengl
	getData(dataBuffer, GL_RGBA, GL_FLOAT);

	char *colorBuffer = new char[txWidth*txHeight*3];
	for (int i = 0; i < txWidth*txHeight; i++)
	{
		char c = (char) (dataBuffer[i*4] / 100.f * 256);
		colorBuffer[i*3+0] = c;
		colorBuffer[i*3+1] = c;
		colorBuffer[i*3+2] = c;
	}

	//Flip data for output
	flip_buffer(colorBuffer,txWidth,txHeight);


	//Write image to PNG
	int res =  stbi_write_png(imgName.c_str(), txWidth, txHeight, 3, colorBuffer, sizeof(char)*3*txWidth);
	if(!res)
	{
		cerr << "Could not write to  " << imgName << endl;
	}

	//Cleanup
	delete [] dataBuffer;

	//Bind old texture
	glBindTexture(GL_TEXTURE_2D,backupBoundTexture);

	return res;
}

bool GLTextureWriter::WriteDepthImage(GLint tid, string imgName)
{
	//Backup old openGL state.
	GLint backupBoundTexture;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &backupBoundTexture);

	//Bind texture to buffer
	glBindTexture(GL_TEXTURE_2D,tid);

	//Retrieve width and height
	int txWidth = getTextureWidth();
	int txHeight = getTextureWidth();


	//Allocate buffer
	float * dataBuffer = new float[txWidth*txHeight];

	//Get data from Opengl
	getData(dataBuffer, GL_DEPTH_COMPONENT, GL_FLOAT);

	char * colorBuffer = new char[txWidth*txHeight*3];
	float maxD = 0;
	for (int i = 0; i < txWidth*txHeight*3; i++)
	{
		colorBuffer[i] = (char) (dataBuffer[i/3] * 256);
	}

	//Flip data for output
	flip_buffer(colorBuffer,txWidth,txHeight);


	//Write image to PNG
	int res =  stbi_write_png(imgName.c_str(), txWidth, txHeight, 3, colorBuffer, sizeof(char)*3*txWidth);
	if(!res)
	{
		cerr << "Could not write to  " << imgName << endl;
	}

	//Cleanup
	delete [] dataBuffer;

	//Bind old texture
	glBindTexture(GL_TEXTURE_2D,backupBoundTexture);

	return res;
}