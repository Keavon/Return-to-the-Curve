#version 330 core

in float depth;
out vec4 value;

void main()
{
    value = vec4(depth);
}

