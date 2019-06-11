#version 330 core
layout(location = 0) in vec2 vertTex;
layout(location = 1) in vec3 vertPos;

out vec2 uv;

uniform mat4 M;

void main()
{
	vec4 pos = M * vec4(vertPos,1.0);
	gl_Position = pos;
	uv = vertTex;	
}
