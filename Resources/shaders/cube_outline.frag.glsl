#version 330 core 

in vec3 fragPos;
uniform mat4 M;
uniform float edge;

out vec4 color;

void main()
{
    float thickness = 0.1 / edge;
    float max = 1 - thickness;
    float min = -1 + thickness;
    bool shouldDiscard = 
        ((fragPos.x > max || fragPos.x < min) && fragPos.y < max && fragPos.y > min && fragPos.z < max && fragPos.z > min) ||
        ((fragPos.y > max || fragPos.y < min) && fragPos.x < max && fragPos.x > min && fragPos.z < max && fragPos.z > min) ||
        ((fragPos.z > max || fragPos.z < min) && fragPos.x < max && fragPos.x > min && fragPos.y < max && fragPos.y > min);

    if (shouldDiscard)
    {
        discard;
    }
    else
    {
	    color = vec4(1, 0, 0, 1);
    }
}
