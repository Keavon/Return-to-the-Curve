#version 330 core
layout(location = 0) in vec3 vertPos;
layout(location = 2) in vec2 vertTex;

uniform mat4 M;
out vec3 vertex_pos;
out vec2 vertex_tex;
void main()
{
	vec4 pos = M * vec4(vertPos,1.0);
	gl_Position = pos;
	vertex_tex = vertTex;	
}
