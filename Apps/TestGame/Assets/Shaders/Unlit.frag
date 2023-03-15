#version 330 core

out vec4 FragColour;

in vec3 outNormals;
in vec2 outTexCoords;

uniform sampler2D albedoMap;

void main()
{
	vec4 albedo = texture(albedoMap, outTexCoords);

	FragColour = vec4(albedo.rgb, 1.0);
	FragColour = vec4(outNormals, 1.0);
	// FragColour = vec4(outTexCoords, 0.0, 1.0);
}