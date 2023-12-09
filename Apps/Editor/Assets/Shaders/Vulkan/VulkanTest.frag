#version 450

layout(location = 0) out vec4 outColour;
layout(location = 0) in vec2 texCoords;

layout(binding = 1) uniform sampler2D texSampler;

void main()
{
	outColour = texture(texSampler, texCoords);
}
