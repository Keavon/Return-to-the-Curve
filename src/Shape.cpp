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
		unsigned int v = eleBuf[i*3+j];
		face.push_back(vec3(M * vec4(posBuf[v*3], posBuf[v*3+1], posBuf[v*3+2], 1.0f)));
	}
	return face;
}

int Shape::getNumFaces()
{
	return eleBuf.size() / 3;
}

vec3 Shape::getVertex(int i, const mat4 &M)
{
	return vec3(M * vec4(posBuf[i*3], posBuf[i*3+1], posBuf[i*3+2], 1.0f));
}

int Shape::getNumVertices()
{
	return posBuf.size() / 3;
}

vector<vec3> Shape::getEdge(int i, const mat4 &M)
{
	vector<vec3> edge;
	for (int j = 0; j < 2; j++)
	{
		unsigned int e = edgeBuf[i*2+j];
		edge.push_back(vec3(M * vec4(posBuf[e*3], posBuf[e*3+1], posBuf[e*3+2], 1.0f)));
	}
	return edge;
}

int Shape::getNumEdges()
{
	return edgeBuf.size() / 2;
}

typedef pair<unsigned int, unsigned int> vert_pair;
struct pair_hash
{
	template <class T1, class T2>
	size_t operator() (const pair<T1, T2> &pair) const
	{
		return hash<T1>()(pair.first) ^ hash<T2>()(pair.second);
	}
};

void Shape::findEdges()
{
	unordered_set<vert_pair, pair_hash> edgeSet;
	for (int i = 0; i < eleBuf.size()/3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			unsigned int v1 = eleBuf[i*3+j];
			unsigned int v2 = eleBuf[i*3+((j+1)%3)];
			vert_pair pair = make_pair(std::min(v1, v2), std::max(v1, v2));
			edgeSet.insert(pair);
		}
	}

	for (vert_pair pair : edgeSet)
	{
		edgeBuf.push_back(pair.first);
		edgeBuf.push_back(pair.second);
	}

}

void Shape::calcNormals()
{
	norBuf.clear();
	norBuf.assign(posBuf.size(), 0.0f);
	for (int i = 0; i < eleBuf.size()/3; i++)
	{
		int f1 = eleBuf[i*3];
		int f2 = eleBuf[i*3+1];
		int f3 = eleBuf[i*3+2];

		vec3 v1 = vec3(posBuf[f1*3], posBuf[f1*3+1], posBuf[f1*3+2]);
		vec3 v2 = vec3(posBuf[f2*3], posBuf[f2*3+1], posBuf[f2*3+2]);
		vec3 v3 = vec3(posBuf[f3*3], posBuf[f3*3+1], posBuf[f3*3+2]);
		
		vec3 a = v2 - v1;
		vec3 b = v3 - v1;

		vec3 crossProduct = cross(a, b);

		norBuf[f1*3] += crossProduct.x;
		norBuf[f1*3+1] += crossProduct.y;
		norBuf[f1*3+2] += crossProduct.z;
		
		norBuf[f2*3] += crossProduct.x;
		norBuf[f2*3+1] += crossProduct.y;
		norBuf[f2*3+2] += crossProduct.z;

		norBuf[f3*3] += crossProduct.x;
		norBuf[f3*3+1] += crossProduct.y;
		norBuf[f3*3+2] += crossProduct.z;
	}

	for (int i = 0; i < norBuf.size()/3; i++)
	{
		vec3 nor = normalize(vec3(norBuf[i*3], norBuf[i*3+1], norBuf[i*3+2]));
		norBuf[i*3] = nor.x;
		norBuf[i*3+1] = nor.y;
		norBuf[i*3+2] = nor.z;
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

	if (! rc)
	{
		cerr << errStr << endl;
	}
	else if (shapes.size())
	{
		posBuf = shapes[0].mesh.positions;
		norBuf = shapes[0].mesh.normals;
		texBuf = shapes[0].mesh.texcoords;
		eleBuf = shapes[0].mesh.indices;
	}
}

void Shape::measure() {
  	float minX, minY, minZ;
	float maxX, maxY, maxZ;

	minX = minY = minZ = 1.1754E+38F;
	maxX = maxY = maxZ = -1.1754E+38F;

	//Go through all vertices to determine min and max of each dimension
	for (size_t v = 0; v < posBuf.size() / 3; v++)
	{
		if(posBuf[3*v+0] < minX) minX = posBuf[3*v+0];
		if(posBuf[3*v+0] > maxX) maxX = posBuf[3*v+0];

		if(posBuf[3*v+1] < minY) minY = posBuf[3*v+1];
		if(posBuf[3*v+1] > maxY) maxY = posBuf[3*v+1];

		if(posBuf[3*v+2] < minZ) minZ = posBuf[3*v+2];
		if(posBuf[3*v+2] > maxZ) maxZ = posBuf[3*v+2];
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
	for (size_t v = 0; v < posBuf.size() / 3; v++)
	{
		if (posBuf[3*v+0] < minX) minX = posBuf[3*v+0];
		if (posBuf[3*v+0] > maxX) maxX = posBuf[3*v+0];

		if (posBuf[3*v+1] < minY) minY = posBuf[3*v+1];
		if (posBuf[3*v+1] > maxY) maxY = posBuf[3*v+1];

		if (posBuf[3*v+2] < minZ) minZ = posBuf[3*v+2];
		if (posBuf[3*v+2] > maxZ) maxZ = posBuf[3*v+2];
	}

	// From min and max compute necessary scale and shift for each dimension
	float maxExtent, xExtent, yExtent, zExtent;
	xExtent = maxX-minX;
	yExtent = maxY-minY;
	zExtent = maxZ-minZ;
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
	for (size_t v = 0; v < posBuf.size() / 3; v++)
	{
		posBuf[3*v+0] = (posBuf[3*v+0] - shift.x) * scale.x;
		assert(posBuf[3*v+0] >= -1.0f - epsilon);
		assert(posBuf[3*v+0] <= 1.0f + epsilon);
		posBuf[3*v+1] = (posBuf[3*v+1] - shift.y) * scale.y;
		assert(posBuf[3*v+1] >= -1.0f - epsilon);
		assert(posBuf[3*v+1] <= 1.0f + epsilon);
		posBuf[3*v+2] = (posBuf[3*v+2] - shift.z) * scale.z;
		assert(posBuf[3*v+2] >= -1.0f - epsilon);
		assert(posBuf[3*v+2] <= 1.0f + epsilon);
	}
}

void Shape::init()
{
	// Initialize the vertex array object
	glGenVertexArrays(1, &vaoID);
	glBindVertexArray(vaoID);

	// Send the position array to the GPU
	glGenBuffers(1, &posBufID);
	glBindBuffer(GL_ARRAY_BUFFER, posBufID);
	glBufferData(GL_ARRAY_BUFFER, posBuf.size()*sizeof(float), posBuf.data(), GL_STATIC_DRAW);

	// Send the normal array to the GPU
	if (norBuf.empty())
	{
		norBufID = 0;
	}
	else
	{
		glGenBuffers(1, &norBufID);
		glBindBuffer(GL_ARRAY_BUFFER, norBufID);
		glBufferData(GL_ARRAY_BUFFER, norBuf.size()*sizeof(float), norBuf.data(), GL_STATIC_DRAW);
	}

	// Send the texture array to the GPU
	if (texBuf.empty())
	{
		texBufID = 0;
	}
	else
	{
		glGenBuffers(1, &texBufID);
		glBindBuffer(GL_ARRAY_BUFFER, texBufID);
		glBufferData(GL_ARRAY_BUFFER, texBuf.size()*sizeof(float), texBuf.data(), GL_STATIC_DRAW);
	}

	// Send the element array to the GPU
	glGenBuffers(1, &eleBufID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eleBufID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, eleBuf.size()*sizeof(unsigned int), eleBuf.data(), GL_STATIC_DRAW);

	// Unbind the arrays
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Shape::draw(const shared_ptr<Program> prog) const
{
	int h_pos, h_nor, h_tex;
	h_pos = h_nor = h_tex = -1;

	glBindVertexArray(vaoID);
	// Bind position buffer
	h_pos = prog->getAttribute("vertPos");
	GLSL::enableVertexAttribArray(h_pos);
	glBindBuffer(GL_ARRAY_BUFFER, posBufID);
	glVertexAttribPointer(h_pos, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);

	// Bind normal buffer
	if (norBufID != 0)
	{
		h_nor = prog->getAttribute("vertNor");
		if (h_nor != -1)
		{
			GLSL::enableVertexAttribArray(h_nor);
			glBindBuffer(GL_ARRAY_BUFFER, norBufID);
			glVertexAttribPointer(h_nor, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);
		}
	}

	if (texBufID != 0)
	{
		// Bind texcoords buffer
		h_tex = prog->getAttribute("vertTex");
		if (h_tex != -1)
		{
			GLSL::enableVertexAttribArray(h_tex);
			glBindBuffer(GL_ARRAY_BUFFER, texBufID);
			glVertexAttribPointer(h_tex, 2, GL_FLOAT, GL_FALSE, 0, (const void *)0);
		}
	}

	// Bind element buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eleBufID);

	// Draw
	glDrawElements(GL_TRIANGLES, (int)eleBuf.size(), GL_UNSIGNED_INT, (const void *)0);

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
