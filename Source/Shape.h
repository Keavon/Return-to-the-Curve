
#pragma once

#ifndef LAB471_SHAPE_H_INCLUDED
#define LAB471_SHAPE_H_INCLUDED

#include "Texture.h"
#include <string>
#include <vector>
#include <memory>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <unordered_set>
#include <iostream>

#include "GLSL.h"
#include "Program.h"

using namespace std;
using namespace glm;

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
	void draw(const std::shared_ptr<Program> program) const;

	glm::vec3 min;
	glm::vec3 max;
	glm::vec3 center;
	glm::vec3 size;

	std::vector<unsigned int> edgeBuffer;

	std::vector<unsigned int> indexBuffer;
	unsigned int indexBufferID = 0;

	std::vector<float> positionBuffer;
	unsigned int positionBufferID = 0;

	std::vector<float> normalBuffer;
	unsigned int normalBufferID = 0;

	std::vector<float> uvBuffer;
	unsigned int uvBufferID = 0;

	unsigned int vaoID = 0;
};

#endif // LAB471_SHAPE_H_INCLUDED
