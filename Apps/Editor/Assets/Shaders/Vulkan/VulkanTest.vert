// Compile with:
// clear; glslc VulkanTest.vert -o vert.spv && glslc VulkanTest.frag -o frag.spv 

#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoords;

layout(location = 0) out vec2 outTexCoords;

layout(push_constant) uniform MVP
{
    mat4 Model;
    mat4 View;
    mat4 Proj;
} mvp;

void main()
{
	gl_Position = mvp.Proj * mvp.View * mvp.Model * vec4(inPosition, 1.0);
	outTexCoords = inTexCoords;
}
