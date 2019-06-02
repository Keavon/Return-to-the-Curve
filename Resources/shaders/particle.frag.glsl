#version 330 core 

out vec4 color;

uniform sampler2D alphaTexture;
uniform vec4 pColor;

in vec2 vTexCoord;

void main()
{
	float alpha = texture(alphaTexture, vTexCoord).r;

	color = vec4(pColor.rgb, pColor.a * alpha);
}
