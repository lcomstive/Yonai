#include <glad/glad.h>
#include <AquaEngine/Resource.hpp>
#include <AquaEngine/Graphics/Mesh.hpp>

using namespace glm;
using namespace std;
using namespace AquaEngine;
using namespace AquaEngine::Graphics;

Mesh::Mesh() : m_Vertices(), m_Indices(), m_VAO(GL_INVALID_VALUE), m_VBO(), m_EBO(), m_DrawMode(DrawMode::Triangles) { }

Mesh::Mesh(vector<Vertex> vertices, vector<unsigned int> indices, DrawMode drawMode) : Mesh()
{
	m_DrawMode = drawMode;
	SetData(vertices, indices);
}

Mesh::~Mesh()
{
	if (m_VAO == GL_INVALID_VALUE)
		return; // Not set up

	glDeleteBuffers(1, &m_VBO);
	glDeleteBuffers(1, &m_EBO);
	glDeleteVertexArrays(1, &m_VAO);
}

void Mesh::SetData(std::vector<Vertex> vertices, std::vector<unsigned int> indices)
{
	m_Vertices = vertices;
	m_Indices = indices;
	Setup();
}

void Mesh::Setup()
{
	// Generate buffers
	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_EBO);
	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	// Fill vertex data
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, m_Vertices.size() * sizeof(Vertex), &m_Vertices[0], GL_STATIC_DRAW);

	// Fill index data
	if (m_Indices.size() > 0)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * sizeof(unsigned int), &m_Indices[0], GL_STATIC_DRAW);
	}

	// Vertex data layout
	// Position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	// Normal
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	// Texture Coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

	// Unbind VAO to prevent data being overriden accidentally
	glBindVertexArray(0);
}

void Mesh::Draw()
{
	glBindVertexArray(m_VAO);
	if (m_Indices.size() > 0)
		glDrawElements((GLenum)m_DrawMode, (GLsizei)m_Indices.size(), GL_UNSIGNED_INT, 0);
	else
		glDrawArrays((GLenum)m_DrawMode, 0, (GLint)m_Vertices.size());
	glBindVertexArray(0);
}

ResourceID QuadID = InvalidResourceID;
ResourceID CubeID = InvalidResourceID;
ResourceID SphereID = InvalidResourceID;

const string QuadMeshName   = "Meshes/Primitive/Quad";
const string CubeMeshName   = "Meshes/Primitive/Cube";
const string SphereMeshName = "Meshes/Primitive/Sphere";

ResourceID Mesh::Quad()
{
	if (QuadID != InvalidResourceID)
		return QuadID;

	vector<Vertex> vertices =
	{
		{
			{  1.0f,  1.0f, 0.0f }, // Position
			{  1.0f,  0.0f, 0.0f }, // Normals
			{  1.0f,  1.0f }		// Texture Coordinates
		},
		{
			{  1.0f, -1.0f, 0.0f }, // Position
			{  0.0f,  1.0f, 0.0f }, // Normals
			{  1.0f,  0.0f }		// Texture Coordinates
		},
		{
			{ -1.0f, -1.0f, 0.0f }, // Position
			{  0.0f,  0.0f, 1.0f }, // Normals
			{  0.0f,  0.0f }		// Texture Coordinates
		},
		{
			{ -1.0f,  1.0f, 0.0f }, // Position
			{  0.0f,  0.0f, 1.0f }, // Normals
			{  0.0f,  1.0f }		// Texture Coordinates
		}
	};
	vector<unsigned int> indices =
	{
		0, 1, 3, // Triangle 1
		1, 2, 3  // Triangle 2
	};

	QuadID = Resource::Load<Mesh>(QuadMeshName, vertices, indices);
	return QuadID;
}

ResourceID Mesh::Cube()
{
	if (CubeID != InvalidResourceID)
		return CubeID;

	vector<Vertex> vertices = {
		// back face
		{ { -1.0f, -1.0f, -1.0f }, {  0.0f,  0.0f, -1.0f }, { 0.0f, 0.0f } }, // bottom-left
		{ {  1.0f,  1.0f, -1.0f }, {  0.0f,  0.0f, -1.0f }, { 1.0f, 1.0f } }, // top-right
		{ {  1.0f, -1.0f, -1.0f }, {  0.0f,  0.0f, -1.0f }, { 1.0f, 0.0f } }, // bottom-right         
		{ {  1.0f,  1.0f, -1.0f }, {  0.0f,  0.0f, -1.0f }, { 1.0f, 1.0f } }, // top-right
		{ { -1.0f, -1.0f, -1.0f }, {  0.0f,  0.0f, -1.0f }, { 0.0f, 0.0f } }, // bottom-left
		{ { -1.0f,  1.0f, -1.0f }, {  0.0f,  0.0f, -1.0f }, { 0.0f, 1.0f } }, // top-left
		// front face
		{ { -1.0f, -1.0f,  1.0f }, {  0.0f,  0.0f,   1.0f }, { 0.0f, 0.0f } }, // bottom-left
		{ {  1.0f, -1.0f,  1.0f }, {  0.0f,  0.0f,   1.0f }, { 1.0f, 0.0f } }, // bottom-right
		{ {  1.0f,  1.0f,  1.0f }, {  0.0f,  0.0f,   1.0f }, { 1.0f, 1.0f } }, // top-right
		{ {  1.0f,  1.0f,  1.0f }, {  0.0f,  0.0f,   1.0f }, { 1.0f, 1.0f } }, // top-right
		{ { -1.0f,  1.0f,  1.0f }, {  0.0f,  0.0f,   1.0f }, { 0.0f, 1.0f } }, // top-left
		{ { -1.0f, -1.0f,  1.0f }, {  0.0f,  0.0f,   1.0f }, { 0.0f, 0.0f } }, // bottom-left
		// left face
		{ { -1.0f,  1.0f,  1.0f }, { -1.0f,  0.0f,  0.0f }, { 1.0f, 0.0f } }, // top-right
		{ { -1.0f,  1.0f, -1.0f }, { -1.0f,  0.0f,  0.0f }, { 1.0f, 1.0f } }, // top-left
		{ { -1.0f, -1.0f, -1.0f }, { -1.0f,  0.0f,  0.0f }, { 0.0f, 1.0f } }, // bottom-left
		{ { -1.0f, -1.0f, -1.0f }, { -1.0f,  0.0f,  0.0f }, { 0.0f, 1.0f } }, // bottom-left
		{ { -1.0f, -1.0f,  1.0f }, { -1.0f,  0.0f,  0.0f }, { 0.0f, 0.0f } }, // bottom-right
		{ { -1.0f,  1.0f,  1.0f }, { -1.0f,  0.0f,  0.0f }, { 1.0f, 0.0f } }, // top-right
		// right face
		{ {  1.0f,  1.0f,  1.0f }, {  1.0f,  0.0f,  0.0f }, { 1.0f, 0.0f } }, // top-left
		{ {  1.0f, -1.0f, -1.0f }, {  1.0f,  0.0f,  0.0f }, { 0.0f, 1.0f } }, // bottom-right
		{ {  1.0f,  1.0f, -1.0f }, {  1.0f,  0.0f,  0.0f }, { 1.0f, 1.0f } }, // top-right         
		{ {  1.0f, -1.0f, -1.0f }, {  1.0f,  0.0f,  0.0f }, { 0.0f, 1.0f } }, // bottom-right
		{ {  1.0f,  1.0f,  1.0f }, {  1.0f,  0.0f,  0.0f }, { 1.0f, 0.0f } }, // top-left
		{ {  1.0f, -1.0f,  1.0f }, {  1.0f,  0.0f,  0.0f }, { 0.0f, 0.0f } }, // bottom-left     
		// bottom face
		{ { -1.0f, -1.0f, -1.0f }, {  0.0f, -1.0f,  0.0f }, { 0.0f, 1.0f } }, // top-right
		{ {  1.0f, -1.0f, -1.0f }, {  0.0f, -1.0f,  0.0f }, { 1.0f, 1.0f } }, // top-left
		{ {  1.0f, -1.0f,  1.0f }, {  0.0f, -1.0f,  0.0f }, { 1.0f, 0.0f } }, // bottom-left
		{ {  1.0f, -1.0f,  1.0f }, {  0.0f, -1.0f,  0.0f }, { 1.0f, 0.0f } }, // bottom-left
		{ { -1.0f, -1.0f,  1.0f }, {  0.0f, -1.0f,  0.0f }, { 0.0f, 0.0f } }, // bottom-right
		{ { -1.0f, -1.0f, -1.0f }, {  0.0f, -1.0f,  0.0f }, { 0.0f, 1.0f } }, // top-right
		// top face
		{ { -1.0f,  1.0f, -1.0f }, {  0.0f,  1.0f,  0.0f }, { 0.0f, 1.0f } }, // top-left
		{ {  1.0f,  1.0f , 1.0f }, {  0.0f,  1.0f,  0.0f }, { 1.0f, 0.0f } }, // bottom-right
		{ {  1.0f,  1.0f, -1.0f }, {  0.0f,  1.0f,  0.0f }, { 1.0f, 1.0f } }, // top-right     
		{ {  1.0f,  1.0f,  1.0f }, {  0.0f,  1.0f,  0.0f }, { 1.0f, 0.0f } }, // bottom-right
		{ { -1.0f,  1.0f, -1.0f }, {  0.0f,  1.0f,  0.0f }, { 0.0f, 1.0f } }, // top-left
		{ { -1.0f,  1.0f,  1.0f }, {  0.0f,  1.0f,  0.0f }, { 0.0f, 0.0f } }// bottom-left        
	};

	CubeID = Resource::Load<Mesh>(CubeMeshName, vertices, vector<unsigned int>());
	return CubeID;
}

ResourceID Mesh::Sphere()
{
	if (SphereID != InvalidResourceID)
		return SphereID;

	vector<Vertex> vertices;
	vector<unsigned int> indices;

	const unsigned int segmentsX = 64;
	const unsigned int segmentsY = 64;
	const float PI = 3.14159265359f;
	for (unsigned int x = 0; x <= segmentsX; x++)
	{
		for (unsigned int y = 0; y <= segmentsY; y++)
		{
			float xSegment = (float)x / (float)segmentsX;
			float ySegment = (float)y / (float)segmentsY;
			float xPos = cos(xSegment * 2.0f * PI) * sin(ySegment * PI);
			float yPos = cos(ySegment * PI);
			float zPos = sin(xSegment * 2.0f * PI) * sin(ySegment * PI);

			vertices.emplace_back(Vertex
			{
				{ xPos, yPos, zPos },  // Position
				{ xPos, yPos, zPos },  // Normal
				{ xSegment, ySegment } // UVs
			});
		}
	}

	bool oddRow = false;
	for (unsigned int y = 0; y < segmentsY; y++)
	{
		if (!oddRow)
		{
			for (unsigned int x = 0; x <= segmentsX; x++)
			{
				indices.emplace_back(y * (segmentsX + 1) + x);
				indices.emplace_back((y + 1) * (segmentsX + 1) + x);
			}
		}
		else
		{
			for (int x = segmentsX; x >= 0; x--)
			{
				indices.emplace_back((y + 1) * (segmentsX + 1) + x);
				indices.emplace_back(y * (segmentsX + 1) + x);
			}
		}
		oddRow = !oddRow;
	}

	SphereID = Resource::Load<Mesh>(SphereMeshName, vertices, indices, DrawMode::TriangleStrip);
	return SphereID;
}