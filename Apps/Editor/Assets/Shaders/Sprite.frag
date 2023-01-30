#version 330 core

out vec4 fragColour;

in vec2 passTexCoords;

uniform sampler2D inputTexture;
uniform vec4 colour = vec4(1.0);

void main()
{
	fragColour = texture(inputTexture, passTexCoords) * colour;
}
