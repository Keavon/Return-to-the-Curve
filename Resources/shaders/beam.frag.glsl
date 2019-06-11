#version 330 core

uniform sampler2D depthBuf;
uniform vec2 viewport;

uniform float density;

in float frontZ;

out vec4 color;

void main()
{
	float backZ = texture(depthBuf, gl_FragCoord.xy / viewport).r;
	float lengthInBeam = backZ - frontZ;
	color = vec4(1, 1, 0, lengthInBeam * density);
}
