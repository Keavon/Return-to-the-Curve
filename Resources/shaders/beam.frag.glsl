#version 330 core

uniform sampler2D depthBuf;
uniform vec2 viewport;

uniform float density = 0.01;

uniform float zNear = 0.1;
uniform float zFar = 200;

in float depth;

out vec4 color;

void main()
{
	color = vec4(-depth / 50, 0, 0, 1);
	return;

	float backZ = texture(depthBuf, gl_FragCoord.xy / viewport).r;
	float frontZ = gl_FragCoord.z / gl_FragCoord.w;
	if (frontZ <= backZ)
	{
		color = vec4(0, 1, 1, (backZ - frontZ) * density);
	}
	else if (backZ == 0)
	{
		color = vec4(0, 1, 0, 1);
	}
	else
	{
		// discard;
		color = vec4((backZ - 0.99) * 100, 0, 0, 1);
	}
}
