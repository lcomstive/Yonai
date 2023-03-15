#version 330 core
#include "assets://Shaders/Include/Camera.inc"

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normals;
layout(location = 2) in vec2 texCoords;

uniform mat4 modelMatrix;

out vec2 outTexCoords;
out vec3 outNormals;

void main()
{
	gl_Position = 	camera.ProjectionMatrix *
					camera.ViewMatrix *
					modelMatrix *
					vec4(position, 1.0);

	outNormals = normals;
	outTexCoords = texCoords;
}