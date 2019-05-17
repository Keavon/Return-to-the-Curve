#version 330 core 
uniform sampler2D Texture0;
uniform sampler2D shadowDepth;

uniform vec3 MatSpec;
uniform vec3 MatAmb;
uniform float Shine;

uniform vec3 lightPos;
uniform vec3 dirLightColor;
uniform vec3 viewPos;

uniform float shadowSize;
float shadowAA = 8.0f;
uniform bool shadows;

out vec4 Outcolor;

in OUT_struct {
   vec3 fPos;
   vec3 fragNor;
   vec2 vTexCoord;
   vec4 fPosLS;
} in_struct;

// returns 1 if shadowed
// called with the point projected into the light's coordinate space
float TestShadow(vec4 LSfPos)
{
	// 1: shift the coordinates from -1, 1 to 0 ,1
	vec3 shifted = (LSfPos.xyz + vec3(1)) * 0.5;

	// 2: read off the stored depth (.) from the ShadowDepth, using the shifted.xy 
	float Ldepth = 0.0f;
	float increment = (1.0f / 512.0f);

	Ldepth = texture(shadowDepth, shifted.xy).r;
		
	// 3: compare to the current depth (.z) of the projected depth
	if (Ldepth < shifted.z - 0.0005)
	{
		//4: return 1 if the point is shadowed
		return 0.8f;
	}	

	return 0.0f;
}

void main()
{
	float Shade;

	//normals
	vec3 normal = normalize(in_struct.fragNor);
	vec3 dirLightDirNorm = normalize(lightPos);

	//diffuse
	vec3 texColor0 = vec3(texture(Texture0, in_struct.vTexCoord));
	vec3 diffuse = texColor0 * max(0, dot(normal, dirLightDirNorm)) * dirLightColor;

	//ambient
	vec3 ambient = MatAmb * texColor0 * dirLightColor;

	//specular
	vec3 viewDir = normalize(viewPos - in_struct.fPos);
	vec3 H = normalize((dirLightDirNorm + viewDir) / 2.0);
	vec3 specular = MatSpec * pow(max(0, dot(H, normal)), Shine) * dirLightColor;

	
	Shade = 0.0f;
	//shadow
	if (shadows)
	{
		float increment = (1.0f / shadowSize);
		float halfIncrement = (increment / 2.0f);

		float startX = (in_struct.fPosLS.x - ((shadowAA + 1.0f) * halfIncrement));
		float startY = (in_struct.fPosLS.y - ((shadowAA + 1.0f) * halfIncrement));

		vec4 startSample = vec4(startX, startY, in_struct.fPosLS.z, in_struct.fPosLS.w);
		vec4 subSample = startSample;

		for (float i = 0.0f; i < shadowAA; i++)
		{
			for (float j = 0.0f; j < shadowAA; j++)
			{
				subSample.x = startX + (i * increment);
				subSample.y = startY +  (j * increment);
				Shade += TestShadow(subSample);
			}	
		}
		Shade = Shade / (shadowAA * shadowAA);
	}
	
	
	diffuse = (diffuse * (1.0 - Shade));

	Outcolor = vec4(diffuse + ambient + specular, 1.0);
}
