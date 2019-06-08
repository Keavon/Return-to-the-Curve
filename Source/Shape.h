
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
	vector<vec3> getFace(int i, const mat4 &M);
	int getNumFaces();
	vec3 getVertex(int i, const mat4 &M);
	int getNumVertices();
	vector<vec3> getEdge(int i, const mat4 &M);
	int getNumEdges();

	void loadMesh(const string &meshName);
	void findEdges();
	void calcNormals();
	void init();
	void resize();
	void measure();
	void draw(const shared_ptr<Program> program) const;

	vec3 min;
	vec3 max;
	vec3 center;
	vec3 size;

	vector<unsigned int> edgeBuffer;

	vector<unsigned int> indexBuffer;
	unsigned int indexBufferID = 0;

	vector<float> positionBuffer;
	unsigned int positionBufferID = 0;

	vector<float> normalBuffer;
	unsigned int normalBufferID = 0;

	vector<float> uvBuffer;
	unsigned int uvBufferID = 0;

	unsigned int vaoID = 0;
};

#endif // LAB471_SHAPE_H_INCLUDED
