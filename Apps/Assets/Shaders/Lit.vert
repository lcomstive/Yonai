#version 330 core
#include "/Assets/Shaders/Include/Camera.inc"

layout(location = 0) in vec3 position;

uniform mat4 modelMatrix;

void main()
{
	gl_Position = camera.ProjectionMatrix * camera.ViewMatrix * modelMatrix * vec4(position, 1.0);
}