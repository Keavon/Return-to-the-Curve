#version 330 core

uniform int objectIndex;

out vec4 value;

void main()
{
	value = vec4(vec3(objectIndex / 100.0f), 1);
}
