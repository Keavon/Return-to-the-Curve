#version  330 core
layout(location = 0) in vec4 vertPos;

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

out float frontZ;

void main()
{
    gl_Position = P * V * M * vertPos;

    frontZ = -(V * M * vertPos).z;
}
