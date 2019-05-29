#version 330 core 

in vec3 fragPos;
uniform mat4 M;
uniform float radius;

out vec4 color;

void main()
{
    float d = length(fragPos);
    float thickness = 0.1 / radius;
    if (d < 1 && d > 1 - thickness)
    {
	    color = vec4(1, 1, 0, 1);
    }
    else
    {
        discard;
    }
}
