#version 330 core

out vec4 fragColour;

in vec2 passTexCoords;

uniform sampler2D inputTexture;
uniform vec4 colour = vec4(1.0);

uniform float time = 0;

void main()
{
	float offset = sin(time - passTexCoords.x) / 2 + 0.65;
	fragColour = texture(inputTexture, passTexCoords) * colour * offset;
}
