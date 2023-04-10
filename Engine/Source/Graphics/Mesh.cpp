#include <glad/glad.h>
#include <AquaEngine/Resource.hpp>
#include <AquaEngine/Graphics/Mesh.hpp>

using namespace glm;
using namespace std;
using namespace AquaEngine;
using namespace AquaEngine::Graphics;

Mesh::Mesh() : m_Vertices(), m_Indices(), m_VAO(GL_INVALID_VALUE), m_VBO(), m_EBO(), m_DrawMode(DrawMode::Triangles)
{
	Setup();
}

Mesh::Mesh(vector<Vertex> vertices, vector<unsigned int> indices, DrawMode drawMode) : Mesh()
{
	Import(vertices, indices, drawMode);
}

Mesh::~Mesh()
{
	if (m_VAO == GL_INVALID_VALUE)
		return; // Not set up

	glDeleteBuffers(1, &m_VBO);
	glDeleteBuffers(1, &m_EBO);
	glDeleteVertexArrays(1, &m_VAO);
}

void Mesh::Import(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, DrawMode drawMode)
{
	m_DrawMode = drawMode;

	SetIndices(indices);
	SetVertices(vertices);
}

void Mesh::Setup()
{
	// Generate buffers
	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_EBO);
	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);

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

vector<Mesh::Vertex>& Mesh::GetVertices() { return m_Vertices; }
vector<unsigned int>& Mesh::GetIndices() { return m_Indices; }

void Mesh::SetVertices(vector<Vertex>& vertices)
{
	m_Vertices = vertices;

	glBindVertexArray(m_VAO);
	glBufferData(GL_ARRAY_BUFFER, m_Vertices.size() * sizeof(Vertex), &m_Vertices[0], GL_STATIC_DRAW);
	glBindVertexArray(0);
}

void Mesh::SetIndices(vector<unsigned int>& indices)
{
	m_Indices = indices;
	if (m_Indices.empty())
		return;

	glBindVertexArray(m_VAO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * sizeof(unsigned int), &m_Indices[0], GL_STATIC_DRAW);
	glBindVertexArray(0);
}

ResourceID QuadID = InvalidResourceID;
ResourceID CubeID = InvalidResourceID;
ResourceID SphereID = InvalidResourceID;

const string QuadMeshName   = "_Internal/ManagedMeshes/Primitive/Quad";
const string CubeMeshName   = "_Internal/ManagedMeshes/Primitive/Cube";
const string SphereMeshName = "_Internal/ManagedMeshes/Primitive/Sphere";

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

#pragma region Internal Calls
#include <AquaEngine/Scripting/ScriptEngine.hpp>
#include <AquaEngine/Scripting/InternalCalls.hpp>

ADD_MANAGED_METHOD(Mesh, Load, void, (MonoString* pathRaw, uint64_t* outID, void** outHandle), AquaEngine.Graphics)
{
	if (*outID == InvalidResourceID)
		*outID = ResourceID(); // Assign resource ID

	char* path = mono_string_to_utf8(pathRaw);
	Resource::Load<Mesh>(*outID, path);
	*outHandle = Resource::Get<Mesh>(*outID);
}

ADD_MANAGED_METHOD(Mesh, Import, void,
	(void* handle, MonoArray* inVertices, MonoArray* inIndices, unsigned char drawMode),
	AquaEngine.Graphics)
{
	vector<Mesh::Vertex> vertices;
	vector<unsigned int> indices;

	vertices.resize(mono_array_length(inVertices));
	indices.reserve(mono_array_length(inIndices));

	for(size_t i = 0; i < vertices.size(); i++)
		vertices[i] = mono_array_get(inVertices, Mesh::Vertex, i);

	for(size_t i = 0; i < indices.size(); i++)
		indices[i] = mono_array_get(inIndices, unsigned int, i);

	((Mesh*)handle)->Import(vertices, indices, (Mesh::DrawMode)drawMode);
}

#define GET_CLASS(name) Scripting::ScriptEngine::GetCoreAssembly()->GetClassFromName("AquaEngine", #name)

ADD_MANAGED_METHOD(Mesh, GetVertices, void, (void* handle, MonoArray** outPositions, MonoArray** outNormals, MonoArray** outTexCoords), AquaEngine.Graphics)
{
	MonoClass* v3Class = GET_CLASS(Vector3);
	MonoClass* v2Class = GET_CLASS(Vector2);

	Mesh* mesh = (Mesh*)handle;
	vector<Mesh::Vertex> vertices = ((Mesh*)handle)->GetVertices();
	if(vertices.empty())
		return;
	
	*outPositions = mono_array_new(mono_domain_get(), v3Class, vertices.size());
	*outNormals = mono_array_new(mono_domain_get(), v3Class, vertices.size());
	*outTexCoords = mono_array_new(mono_domain_get(), v2Class, vertices.size());
	for(size_t i = 0; i < vertices.size(); i++)
	{
		mono_array_set(*outPositions, glm::vec3, i, vertices[i].Position);
		mono_array_set(*outNormals, glm::vec3, i, vertices[i].Normal);
		mono_array_set(*outTexCoords, glm::vec2, i, vertices[i].TexCoords);
	}
}

ADD_MANAGED_METHOD(Mesh, SetVertices, void, (void* handle, MonoArray* inPositions, MonoArray* inNormals, MonoArray* inTexCoords), AquaEngine.Graphics)
{
	size_t vertexCount = mono_array_length(inPositions);
	vector<Mesh::Vertex> vertices;
	vertices.resize(vertexCount);
	for(size_t i = 0; i < vertexCount; i++)
	{
		vertices[i].Position = mono_array_get(inPositions, glm::vec3, i);
		vertices[i].Normal = mono_array_get(inNormals, glm::vec3, i);
		vertices[i].TexCoords = mono_array_get(inTexCoords, glm::vec2, i);
	}
	
	((Mesh*)handle)->SetVertices(vertices);
}

ADD_MANAGED_METHOD(Mesh, GetIndices, void, (void* handle, MonoArray** outIndices), AquaEngine.Graphics)
{
	vector<unsigned int> indices = ((Mesh*)handle)->GetIndices();
	*outIndices = mono_array_new(mono_domain_get(), mono_get_uint32_class(), indices.size());
	for(size_t i = 0; i < indices.size(); i++)
		mono_array_set(*outIndices, unsigned int, i, indices[i]);
}

ADD_MANAGED_METHOD(Mesh, SetIndices, void, (void* handle, MonoArray* inIndices), AquaEngine.Graphics)
{
	size_t indexCount = mono_array_length(inIndices);
	vector<unsigned int> indices;
	indices.resize(indexCount);
	for(size_t i = 0; i < indexCount; i++)
		indices[i] = mono_array_get(inIndices, unsigned int, i);
	
	((Mesh*)handle)->SetIndices(indices);
}
#pragma endregion