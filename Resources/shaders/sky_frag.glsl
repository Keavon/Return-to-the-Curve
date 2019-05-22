#version 330 core 

out vec4 color;

uniform samplerCube Texture0;

in vec3 vTexCoord;

void main()
{
	color = texture(Texture0, vTexCoord);
}
