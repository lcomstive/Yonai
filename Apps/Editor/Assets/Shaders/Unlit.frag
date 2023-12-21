#version 330 core

out vec4 fragColour;

in vec2 passTexCoords;

uniform int hasAlbedoMap;
uniform sampler2D inputTexture;
uniform vec4 colour = vec4(1.0);

void main()
{
	if(hasAlbedoMap == 1)
	{
		vec4 textureData = texture(inputTexture, passTexCoords);
		fragColour = textureData * colour;
	}
	else
		fragColour = colour;
}
