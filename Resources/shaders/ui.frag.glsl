#version 330 core
out vec4 FragColor;
in vec3 vertex_pos;
in vec2 vertex_tex;

uniform sampler2D tex;
uniform float xOffset;
uniform float yOffset;

void main()
{
	vec4 tcol = texture(tex,vec2(vertex_tex.x, vertex_tex.y));
	FragColor = tcol;
	//FragColor = vec4(0, 0, 1, 1);
}
