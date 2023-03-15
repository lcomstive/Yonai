#version 330 core

out vec4 fragColour;

in vec2 passTexCoords;

uniform sampler2D inputTexture;
uniform vec4 colour = vec4(1.0);

uniform float multiplier = 1.0;

void main()
{
	fragColour = texture(inputTexture, passTexCoords) * colour;

	fragColour *= cos(multiplier) / 4 + 0.75;
}
