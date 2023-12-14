// Compile with:
// clear; glslc VulkanTest.vert -o vert.spv && glslc VulkanTest.frag -o frag.spv 

#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoords;

layout(location = 0) out vec2 outTexCoords;

layout(binding = 0) uniform MVP
{
    mat4 Model;
    mat4 View;
    mat4 Proj;
} mvp;

void main()
{
	const vec3 positions[3] = vec3[3](
		vec3( 1,  1, 0),
		vec3(-1,  1, 0),
		vec3( 0, -1, 0)
	);

	gl_Position = vec4(positions[gl_VertexIndex], 1.0);
	outTexCoords = inTexCoords;
}
