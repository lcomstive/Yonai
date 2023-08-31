#version 330 core

out vec4 FragColour;

in vec3 outPosition;

uniform float time;

void main()
{
	FragColour = vec4(1.0);
	
	// FragColour.rgb *= cos(time + outPosition) * 0.5 + 0.5;
}