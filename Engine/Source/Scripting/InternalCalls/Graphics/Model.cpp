#include <glm/glm.hpp>
#include <assimp/scene.h>
#include <spdlog/spdlog.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <Yonai/Scripting/ScriptEngine.hpp>

using namespace glm;
using namespace std;
using namespace Yonai;
using namespace Assimp;
using namespace Yonai::IO;
using namespace Yonai::Components;

#include <Yonai/Scripting/InternalCalls.hpp>

struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
}; 

struct MeshImportData
{
	// Array of Vertex
	MonoArray* Vertices;
	// Array of unsigned int
	MonoArray* Indices;

	MeshImportData(aiMesh* mesh) : Vertices(nullptr), Indices(nullptr)
	{
		vector<Vertex> vertices;
		vector<unsigned int> indices;

		// Process vertices
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex =
			{
				{ mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z }, // Position
				{ mesh->mNormals[i].x,	mesh->mNormals[i].y,  mesh->mNormals[i].z }
			};

			if (mesh->mTextureCoords[0])
				vertex.TexCoords = { mesh->mTextureCoords[0][i].x, 1.0f - mesh->mTextureCoords[0][i].y };
			else
				vertex.TexCoords = { 0, 0 };

			vertices.emplace_back(vertex);
		}

		// Process indices
		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
			for (unsigned int j = 0; j < mesh->mFaces[i].mNumIndices; j++)
				indices.emplace_back(mesh->mFaces[i].mIndices[j]);

		MonoClass* vertexClass = mono_class_from_name(Scripting::ScriptEngine::GetCoreAssembly()->Image, "Yonai.Graphics", "Mesh/Vertex");
		Vertices = mono_array_new(mono_domain_get(), vertexClass, (unsigned int)vertices.size());
		Indices = mono_array_new(mono_domain_get(), mono_get_uint32_class(), (unsigned int)indices.size());

		for (size_t i = 0; i < vertices.size(); i++)
			mono_array_set(Vertices, Vertex, i, vertices[i]);
		for (size_t i = 0; i < indices.size(); i++)
			mono_array_set(Indices, unsigned int, i, indices[i]);
	}
};

ADD_MANAGED_METHOD(Model, Import, bool, (
	MonoArray* modelData,
	bool importMaterials,
	MonoArray** outMeshes
), Yonai.Graphics)
{
	vector<unsigned char> data;
	data.resize(mono_array_length(modelData));
	for (size_t i = 0; i < data.size(); i++)
		data[i] = mono_array_get(modelData, unsigned char, i);

	// Check for empty data
	if (data.empty())
		return false;
	
	Importer importer;

	unsigned int postProcessing = aiProcessPreset_TargetRealtime_MaxQuality;
	const aiScene* scene = importer.ReadFileFromMemory(data.data(), data.size(), postProcessing);

	if (!scene)
	{
		spdlog::error("Failed to load model - {}", importer.GetErrorString());
		return false;
	}

	MonoClass* meshImportClass = Scripting::ScriptEngine::GetCoreAssembly()->GetClassFromName("Yonai.Graphics", "MeshImportData");
	*outMeshes = mono_array_new(mono_domain_get(), meshImportClass, scene->mNumMeshes);
	for (unsigned int i = 0; i < scene->mNumMeshes; i++)
	{
		MeshImportData meshData(scene->mMeshes[i]);
		mono_array_set(*outMeshes, MeshImportData, i, meshData);
	}

	return true;
}
