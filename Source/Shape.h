
#pragma once

#ifndef LAB471_SHAPE_H_INCLUDED
#define LAB471_SHAPE_H_INCLUDED

#include "Texture.h"
#include <string>
#include <vector>
#include <memory>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Program;

class Shape
{

public:
	std::vector<glm::vec3> getFace(int i, const glm::mat4 &M);
	int getNumFaces();
	glm::vec3 getVertex(int i, const glm::mat4 &M);
	int getNumVertices();
	std::vector<glm::vec3> getEdge(int i, const glm::mat4 &M);
	int getNumEdges();

	void loadMesh(const std::string &meshName);
	void findEdges();
	void calcNormals();
	void init();
	void resize();
	void measure();
	void draw(const std::shared_ptr<Program> prog) const;

	glm::vec3 min;
	glm::vec3 max;
	glm::vec3 center;
	glm::vec3 size;

// private:

	std::vector<unsigned int> eleBuf;
	std::vector<unsigned int> edgeBuf;
	std::vector<float> posBuf;
	std::vector<float> norBuf;
	std::vector<float> texBuf;

	unsigned int eleBufID = 0;
	unsigned int posBufID = 0;
	unsigned int norBufID = 0;
	unsigned int texBufID = 0;
	unsigned int vaoID = 0;
};

#endif // LAB471_SHAPE_H_INCLUDED
