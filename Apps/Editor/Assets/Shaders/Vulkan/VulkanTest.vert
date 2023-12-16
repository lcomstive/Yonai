// Compile with:
// clear; glslc VulkanTest.vert -o vert.spv && glslc VulkanTest.frag -o frag.spv 

#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoords;

layout(location = 0) out vec2 outTexCoords;

layout(push_constant) uniform MVP
{
    mat4 MVP;
} mvp;

void main()
{
	gl_Position = mvp.MVP * vec4(inPosition, 1.0);
	outTexCoords = inTexCoords;
}
