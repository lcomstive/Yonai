#version 330 core
#include "assets://Shaders/Include/Camera.inc"

layout(location = 0) in vec3 position;
layout(location = 2) in vec2 texCoords;

out vec2 passTexCoords;

uniform mat4 modelMatrix;

void main()
{
	passTexCoords = texCoords;
	gl_Position = camera.ProjectionMatrix * camera.ViewMatrix * modelMatrix * vec4(position, 1.0);
}