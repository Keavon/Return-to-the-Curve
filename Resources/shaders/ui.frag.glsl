#version 330 core
out vec4 FragColor;
in vec2 uv;

uniform sampler2D Texture;

void main()
{
	vec4 tcol = texture(Texture, vec2(uv.y, 1 - uv.x));
	FragColor = tcol;
	//FragColor = vec4(0, 0, 1, 1);
}
