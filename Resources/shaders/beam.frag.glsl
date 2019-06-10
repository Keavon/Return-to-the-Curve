#version 330 core

uniform sampler2D depthBuf;
uniform vec2 viewport;

uniform float density = 0.05;

in float frontZ;

out vec4 color;

void main()
{
	float backZ = texture(depthBuf, gl_FragCoord.xy / viewport).r;
	float lengthInBeam = backZ - max(0, frontZ);
	if (frontZ == 0)
	{
		color = vec4(1, 1, 1, 1);
		return;
	}
	if (lengthInBeam > 0)
	{
		color = vec4(0, 1, 1, lengthInBeam * density);
	}
	else
	{
		color = vec4(1, 0, 0, 1);
	}
}
