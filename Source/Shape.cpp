#include "Shape.h"
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

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include "stb_image.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
using namespace glm;

vector<vec3> Shape::getFace(int i, const mat4 &M)
{
	vector<vec3> face;
	for (int j = 0; j < 3; j++)
	{
		unsigned int v = indexBuffer[i * 3 + j];
		face.push_back(vec3(M * vec4(positionBuffer[v * 3], positionBuffer[v * 3 + 1], positionBuffer[v * 3 + 2], 1.0f)));
	}
	return face;
}

int Shape::getNumFaces()
{
	return indexBuffer.size() / 3;
}

vec3 Shape::getVertex(int i, const mat4 &M)
{
	return vec3(M * vec4(positionBuffer[i * 3], positionBuffer[i * 3 + 1], positionBuffer[i * 3 + 2], 1.0f));
}

int Shape::getNumVertices()
{
	return positionBuffer.size() / 3;
}

vector<vec3> Shape::getEdge(int i, const mat4 &M)
{
	vector<vec3> edge;
	for (int j = 0; j < 2; j++)
	{
		unsigned int e = edgeBuffer[i * 2 + j];
		edge.push_back(vec3(M * vec4(positionBuffer[e * 3], positionBuffer[e * 3 + 1], positionBuffer[e * 3 + 2], 1.0f)));
	}
	return edge;
}

int Shape::getNumEdges()
{
	return edgeBuffer.size() / 2;
}

typedef pair<unsigned int, unsigned int> vert_pair;
struct pair_hash
{
	template <class T1, class T2>
	size_t operator()(const pair<T1, T2> &pair) const
	{
		return hash<T1>()(pair.first) ^ hash<T2>()(pair.second);
	}
};

void Shape::findEdges()
{
	unordered_set<vert_pair, pair_hash> edgeSet;
	for (int i = 0; i < indexBuffer.size() / 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			unsigned int v1 = indexBuffer[i * 3 + j];
			unsigned int v2 = indexBuffer[i * 3 + ((j + 1) % 3)];
			vert_pair pair = make_pair(std::min(v1, v2), std::max(v1, v2));
			edgeSet.insert(pair);
		}
	}

	for (vert_pair pair : edgeSet)
	{
		edgeBuffer.push_back(pair.first);
		edgeBuffer.push_back(pair.second);
	}
}

void Shape::calcNormals()
{
	normalBuffer.clear();
	normalBuffer.assign(positionBuffer.size(), 0.0f);
	for (int i = 0; i < indexBuffer.size() / 3; i++)
	{
		int f1 = indexBuffer[i * 3];
		int f2 = indexBuffer[i * 3 + 1];
		int f3 = indexBuffer[i * 3 + 2];

		vec3 v1 = vec3(positionBuffer[f1 * 3], positionBuffer[f1 * 3 + 1], positionBuffer[f1 * 3 + 2]);
		vec3 v2 = vec3(positionBuffer[f2 * 3], positionBuffer[f2 * 3 + 1], positionBuffer[f2 * 3 + 2]);
		vec3 v3 = vec3(positionBuffer[f3 * 3], positionBuffer[f3 * 3 + 1], positionBuffer[f3 * 3 + 2]);

		vec3 a = v2 - v1;
		vec3 b = v3 - v1;

		vec3 crossProduct = cross(a, b);

		normalBuffer[f1 * 3] += crossProduct.x;
		normalBuffer[f1 * 3 + 1] += crossProduct.y;
		normalBuffer[f1 * 3 + 2] += crossProduct.z;

		normalBuffer[f2 * 3] += crossProduct.x;
		normalBuffer[f2 * 3 + 1] += crossProduct.y;
		normalBuffer[f2 * 3 + 2] += crossProduct.z;

		normalBuffer[f3 * 3] += crossProduct.x;
		normalBuffer[f3 * 3 + 1] += crossProduct.y;
		normalBuffer[f3 * 3 + 2] += crossProduct.z;
	}

	for (int i = 0; i < normalBuffer.size() / 3; i++)
	{
		vec3 nor = normalize(vec3(normalBuffer[i * 3], normalBuffer[i * 3 + 1], normalBuffer[i * 3 + 2]));
		normalBuffer[i * 3] = nor.x;
		normalBuffer[i * 3 + 1] = nor.y;
		normalBuffer[i * 3 + 2] = nor.z;
	}
}

void Shape::loadMesh(const string &meshName)
{
	// Load geometry
	// Some obj files contain material information.
	// We'll ignore them for this assignment.
	vector<tinyobj::shape_t> shapes;
	vector<tinyobj::material_t> objMaterials;
	string errStr;
	bool rc = tinyobj::LoadObj(shapes, objMaterials, errStr, meshName.c_str());

	if (!rc)
	{
		cerr << errStr << endl;
	}
	else if (shapes.size())
	{
		positionBuffer = shapes[0].mesh.positions;
		normalBuffer = shapes[0].mesh.normals;
		uvBuffer = shapes[0].mesh.texcoords;
		indexBuffer = shapes[0].mesh.indices;
	}
}

void Shape::measure()
{
	float minX, minY, minZ;
	float maxX, maxY, maxZ;

	minX = minY = minZ = 1.1754E+38F;
	maxX = maxY = maxZ = -1.1754E+38F;

	//Go through all vertices to determine min and max of each dimension
	for (size_t v = 0; v < positionBuffer.size() / 3; v++)
	{
		if (positionBuffer[3 * v + 0] < minX)
			minX = positionBuffer[3 * v + 0];
		if (positionBuffer[3 * v + 0] > maxX)
			maxX = positionBuffer[3 * v + 0];

		if (positionBuffer[3 * v + 1] < minY)
			minY = positionBuffer[3 * v + 1];
		if (positionBuffer[3 * v + 1] > maxY)
			maxY = positionBuffer[3 * v + 1];

		if (positionBuffer[3 * v + 2] < minZ)
			minZ = positionBuffer[3 * v + 2];
		if (positionBuffer[3 * v + 2] > maxZ)
			maxZ = positionBuffer[3 * v + 2];
	}

	min.x = minX;
	min.y = minY;
	min.z = minZ;
	max.x = maxX;
	max.y = maxY;
	max.z = maxZ;

	center = (max + min) / 2.0f;

	size = max - min;
}

void Shape::resize()
{
	float minX, minY, minZ;
	float maxX, maxY, maxZ;
	vec3 scale, shift;
	float epsilon = 0.001f;

	minX = minY = minZ = 1.1754E+38F;
	maxX = maxY = maxZ = -1.1754E+38F;

	// Go through all vertices to determine min and max of each dimension
	for (size_t v = 0; v < positionBuffer.size() / 3; v++)
	{
		if (positionBuffer[3 * v + 0] < minX)
			minX = positionBuffer[3 * v + 0];
		if (positionBuffer[3 * v + 0] > maxX)
			maxX = positionBuffer[3 * v + 0];

		if (positionBuffer[3 * v + 1] < minY)
			minY = positionBuffer[3 * v + 1];
		if (positionBuffer[3 * v + 1] > maxY)
			maxY = positionBuffer[3 * v + 1];

		if (positionBuffer[3 * v + 2] < minZ)
			minZ = positionBuffer[3 * v + 2];
		if (positionBuffer[3 * v + 2] > maxZ)
			maxZ = positionBuffer[3 * v + 2];
	}

	// From min and max compute necessary scale and shift for each dimension
	float maxExtent, xExtent, yExtent, zExtent;
	xExtent = maxX - minX;
	yExtent = maxY - minY;
	zExtent = maxZ - minZ;
	if (xExtent >= yExtent && xExtent >= zExtent)
	{
		maxExtent = xExtent;
	}
	if (yExtent >= xExtent && yExtent >= zExtent)
	{
		maxExtent = yExtent;
	}
	if (zExtent >= xExtent && zExtent >= yExtent)
	{
		maxExtent = zExtent;
	}
	scale.x = 2.0f / maxExtent;
	shift.x = minX + (xExtent / 2.0f);
	scale.y = 2.0f / maxExtent;
	shift.y = minY + (yExtent / 2.0f);
	scale.z = 2.0f / maxExtent;
	shift.z = minZ + (zExtent / 2.0f);

	// Go through all verticies shift and scale them
	for (size_t v = 0; v < positionBuffer.size() / 3; v++)
	{
		positionBuffer[3 * v + 0] = (positionBuffer[3 * v + 0] - shift.x) * scale.x;
		assert(positionBuffer[3 * v + 0] >= -1.0f - epsilon);
		assert(positionBuffer[3 * v + 0] <= 1.0f + epsilon);
		positionBuffer[3 * v + 1] = (positionBuffer[3 * v + 1] - shift.y) * scale.y;
		assert(positionBuffer[3 * v + 1] >= -1.0f - epsilon);
		assert(positionBuffer[3 * v + 1] <= 1.0f + epsilon);
		positionBuffer[3 * v + 2] = (positionBuffer[3 * v + 2] - shift.z) * scale.z;
		assert(positionBuffer[3 * v + 2] >= -1.0f - epsilon);
		assert(positionBuffer[3 * v + 2] <= 1.0f + epsilon);
	}
}

void Shape::init()
{
	// Initialize the vertex array object
	glGenVertexArrays(1, &vaoID);
	glBindVertexArray(vaoID);

	// Send the position array to the GPU
	glGenBuffers(1, &positionBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, positionBufferID);
	glBufferData(GL_ARRAY_BUFFER, positionBuffer.size() * sizeof(float), positionBuffer.data(), GL_STATIC_DRAW);

	// Send the normal array to the GPU
	if (normalBuffer.empty())
	{
		normalBufferID = 0;
	}
	else
	{
		glGenBuffers(1, &normalBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, normalBufferID);
		glBufferData(GL_ARRAY_BUFFER, normalBuffer.size() * sizeof(float), normalBuffer.data(), GL_STATIC_DRAW);
	}

	// Send the texture array to the GPU
	if (uvBuffer.empty())
	{
		uvBufferID = 0;
	}
	else
	{
		glGenBuffers(1, &uvBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
		glBufferData(GL_ARRAY_BUFFER, uvBuffer.size() * sizeof(float), uvBuffer.data(), GL_STATIC_DRAW);
	}

	// Send the element array to the GPU
	glGenBuffers(1, &indexBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBuffer.size() * sizeof(unsigned int), indexBuffer.data(), GL_STATIC_DRAW);

	// Unbind the arrays
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Shape::draw(const shared_ptr<Program> program) const
{
	int h_pos = -1;
	int h_nor = -1;
	int h_tex = -1;

	glBindVertexArray(vaoID);

	// Bind position buffer
	if (positionBufferID != 0 && program->hasAttribute("vertPos"))
	{
		h_pos = program->getAttribute("vertPos");
		if (h_pos != -1)
		{
			GLSL::enableVertexAttribArray(h_pos);
			glBindBuffer(GL_ARRAY_BUFFER, positionBufferID);
			glVertexAttribPointer(h_pos, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);
		}
	}

	// Bind normal buffer
	if (normalBufferID != 0 && program->hasAttribute("vertNor"))
	{
		h_nor = program->getAttribute("vertNor");
		if (h_nor != -1)
		{
			GLSL::enableVertexAttribArray(h_nor);
			glBindBuffer(GL_ARRAY_BUFFER, normalBufferID);
			glVertexAttribPointer(h_nor, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);
		}
	}

	// Bind texcoords buffer
	if (uvBufferID != 0 && program->hasAttribute("vertTex"))
	{
		h_tex = program->getAttribute("vertTex");
		if (h_tex != -1)
		{
			GLSL::enableVertexAttribArray(h_tex);
			glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
			glVertexAttribPointer(h_tex, 2, GL_FLOAT, GL_FALSE, 0, (const void *)0);
		}
	}

	// Bind element buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);

	// Draw
	glDrawElements(GL_TRIANGLES, (int)indexBuffer.size(), GL_UNSIGNED_INT, (const void *)0);

	// Disable and unbind
	if (h_tex != -1)
	{
		GLSL::disableVertexAttribArray(h_tex);
	}
	if (h_nor != -1)
	{
		GLSL::disableVertexAttribArray(h_nor);
	}
	GLSL::disableVertexAttribArray(h_pos);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
