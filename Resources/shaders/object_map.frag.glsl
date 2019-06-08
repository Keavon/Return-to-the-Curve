#version 330 core

uniform int objectIndex;

out int value;

void main()
{
	value = objectIndex;
}
