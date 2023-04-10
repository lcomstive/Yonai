#include <spdlog/spdlog.h>
#include <assimp/Importer.hpp>
#include <AquaEngine/Timer.hpp>
#include <assimp/postprocess.h>
#include <AquaEngine/IO/VFS.hpp>
#include <AquaEngine/Resource.hpp>
#include <AquaEngine/Graphics/Model.hpp>
#include <AquaEngine/Graphics/Shader.hpp>
#include <AquaEngine/Graphics/Texture.hpp>
#include <AquaEngine/Components/Transform.hpp>
#include <AquaEngine/Components/DebugName.hpp>
#include <AquaEngine/Components/MeshRenderer.hpp>

using namespace glm;
using namespace std;
using namespace Assimp;
using namespace AquaEngine;
using namespace AquaEngine::IO;
using namespace AquaEngine::Graphics;
using namespace AquaEngine::Components;

Model::Model() : m_Path(""), m_Root(), m_MeshIDs() { }
Model::Model(string& path) : Model() { Import(path); }

void Model::Import(const char* path) { Import(string(path)); }
void Model::Import(string path)
{
	if (!m_Path.empty())
		// Release previous model
		m_Root = {};

	m_Path = path;
	Load();
}

void ApplyAssimpTransformation(aiMatrix4x4 transformation, Transform* transform)
{
	aiVector3D scale = {};
	aiQuaternion rotation = {};
	aiVector3D position = {};
	transformation.Decompose(scale, rotation, position);

	transform->Scale = { scale.x, scale.y, scale.z };
	transform->Position = { position.x, position.y, position.z };
	transform->Rotation = eulerAngles(quat{ rotation.w, rotation.x, rotation.y, rotation.z });
}

vector<ResourceID> Model::GetMeshes() { return m_MeshIDs; }

void Model::CreateMeshMaterialPair(const MeshData& data, vector<pair<ResourceID, ResourceID>>& output)
{
	size_t count = std::min(data.MeshIDs.size(), data.MaterialIDs.size());
	output.reserve(count);
	for(size_t i = 0; i < count; i++)
		output.emplace_back(make_pair(m_MeshIDs[data.MeshIDs[i]], data.MaterialIDs[i]));

	for(const MeshData& child : data.Children)
		CreateMeshMaterialPair(child, output);
}

vector<pair<ResourceID, ResourceID>> Model::GetMeshesAndMaterials()
{
	vector<pair<ResourceID, ResourceID>> output;

	CreateMeshMaterialPair(m_Root, output);

	return output;
} 

void Model::Load()
{
	if (m_Path.empty())
		return;

	Importer importer;

	const aiScene* scene = nullptr;
	unsigned int postProcessing = aiProcess_Triangulate | aiProcess_FlipUVs;

	// Read in model file
	vector<unsigned char> modelData = VFS::Read(m_Path);
	if (modelData.size() == 0)
	{
		spdlog::error("Failed to load model '{}' - Could not find file in VFS", m_Path);
		return;
	}
	string extension = filesystem::path(m_Path).extension().string();
	// Log::Debug("TESTING EXTENSION '" + extension + "'");
	scene = importer.ReadFileFromMemory(
		modelData.data(),
		modelData.size(),
		postProcessing,
		extension.c_str());

	// Safety check
	if (!scene)
	{
		spdlog::error("Failed to load model '{}' - {}", m_Path, importer.GetErrorString());
		return;
	}

	// Read in meshes
	m_MeshIDs.reserve(scene->mNumMeshes);
	for (unsigned int i = 0; i < scene->mNumMeshes; i++)
		m_MeshIDs.emplace_back(ProcessMesh(scene->mMeshes[i], scene));

	m_Root = {};

	// Create mesh data hierarchy
	ProcessNode(scene->mRootNode, nullptr, scene);
}

void LoadMaterialTextures(
	string currentDirectory,
	aiTextureType textureType,
	aiMaterial* aiMat,
	ResourceID& outputID)
{
	int textureCount = aiMat->GetTextureCount(textureType);

	if (aiMat->GetTextureCount(textureType) <= 0)
	{
		outputID = InvalidResourceID;
		return;
	}

	aiString aiTexturePath;
	aiMat->GetTexture(textureType, 0, &aiTexturePath);

	string texturePath = currentDirectory + aiTexturePath.C_Str();
	replace(texturePath.begin(), texturePath.end(), '\\', '/');

	outputID = Resource::Load<Texture>(texturePath, texturePath);
}

ResourceID Model::CreateMaterial(aiMaterial* aiMat)
{
	filesystem::path path(m_Path);
	string currentDirectory = path.parent_path().string() + "/"; // Get directory of model path
	string materialName = "Materials/" + path.filename().string() + "/" + aiMat->GetName().C_Str();

	ResourceID materialID = Resource::Load<Material>(materialName);
	Material* material = Resource::Get<Material>(materialID);

	// Albedo / Base Colour
	LoadMaterialTextures(currentDirectory, aiTextureType_DIFFUSE, aiMat, material->AlbedoMap);
	if (material->AlbedoMap == InvalidResourceID)
		LoadMaterialTextures(currentDirectory, aiTextureType_BASE_COLOR, aiMat, material->AlbedoMap);

	LoadMaterialTextures(currentDirectory, aiTextureType_NORMALS, aiMat, material->NormalMap);
	LoadMaterialTextures(currentDirectory, aiTextureType_METALNESS, aiMat, material->MetalnessMap);
	LoadMaterialTextures(currentDirectory, aiTextureType_DIFFUSE_ROUGHNESS, aiMat, material->RoughnessMap);
	LoadMaterialTextures(currentDirectory, aiTextureType_AMBIENT_OCCLUSION, aiMat, material->AmbientOcclusionMap);

	return materialID;
}

void Model::ProcessNode(aiNode* node, MeshData* parent, const aiScene* scene)
{
	MeshData meshData = { node->mName.C_Str(), node->mTransformation };

	// Process meshes in node
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		meshData.MeshIDs.emplace_back(node->mMeshes[i]);

		aiMaterial* material = scene->mMaterials[scene->mMeshes[node->mMeshes[i]]->mMaterialIndex];
		meshData.MaterialIDs.emplace_back(CreateMaterial(material));
	}

	// Process mesh's children
	for (unsigned int i = 0; i < node->mNumChildren; i++)
		ProcessNode(node->mChildren[i], &meshData, scene);

	if (parent)
		parent->Children.emplace_back(meshData);
	else
		m_Root = meshData;
}

ResourceID Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
	vector<Mesh::Vertex> vertices;
	vector<unsigned int> indices;

	// Process vertices
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Mesh::Vertex vertex =
		{
			/* Position */ { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z },
			/* Normals  */ { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z }
		};

		if (mesh->mTextureCoords[0])
			vertex.TexCoords = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
		else
			vertex.TexCoords = { 0, 0 };

		vertices.emplace_back(vertex);
	}

	// Process indices
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		for (unsigned int j = 0; j < mesh->mFaces[i].mNumIndices; j++)
			indices.emplace_back(mesh->mFaces[i].mIndices[j]);

	return Resource::Load<Mesh>(
		m_Path + "/Mesh/" + mesh->mName.C_Str(),
		vertices, indices);
}


#pragma region Internal Calls
#include <AquaEngine/Scripting/Assembly.hpp>
#include <AquaEngine/Scripting/ScriptEngine.hpp>
#include <AquaEngine/Scripting/InternalCalls.hpp>

ADD_MANAGED_METHOD(Model, Load, void, (MonoString* pathRaw, uint64_t* outID, void** outHandle), AquaEngine.Graphics)
{
	if (*outID == InvalidResourceID)
		*outID = ResourceID(); // Assign resource ID

	char* path = mono_string_to_utf8(pathRaw);
	Resource::Load<Model>(*outID, path);
	*outHandle = Resource::Get<Model>(*outID);
	mono_free(path);
}

ADD_MANAGED_METHOD(Model, Import, void, (void* handle, MonoString* filepath), AquaEngine.Graphics)
{ ((Model*)handle)->Import(mono_string_to_utf8(filepath)); }

ADD_MANAGED_METHOD(Model, GetMeshes, void, (void* handle, MonoArray** outMeshIDs, MonoArray** outMaterialIDs), AquaEngine.Graphics)
{
	vector<pair<ResourceID, ResourceID>> meshes = ((Model*)handle)->GetMeshesAndMaterials();
	*outMeshIDs 	= mono_array_new(mono_domain_get(), mono_get_uint64_class(), meshes.size());
	*outMaterialIDs = mono_array_new(mono_domain_get(), mono_get_uint64_class(), meshes.size());
	for(size_t i = 0; i < meshes.size(); i++)
	{
		mono_array_set(*outMeshIDs, uint64_t, i, meshes[i].first);
		mono_array_set(*outMaterialIDs, uint64_t, i, meshes[i].second);
	}
}

#pragma endregion
