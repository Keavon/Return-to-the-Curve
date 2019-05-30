#version  330 core
layout(location = 0) in vec4 vertPos;
layout(location = 1) in vec3 vertNor;
layout(location = 2) in vec2 vertTex;

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

out vec2 vTexCoord;

void main()
{
	vTexCoord = vertTex;
	gl_Position = P * V * M * mat4(inverse(mat3(V))) * vertPos;
}
