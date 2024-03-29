#include <spdlog/spdlog.h>
#include <assimp/Importer.hpp>
#include <Yonai/Timer.hpp>
#include <assimp/postprocess.h>
#include <Yonai/Resource.hpp>
#include <Yonai/Graphics/Model.hpp>
#include <Yonai/Graphics/Shader.hpp>
#include <Yonai/Graphics/Texture.hpp>
#include <Yonai/Components/Transform.hpp>
#include <Yonai/Components/DebugName.hpp>
#include <Yonai/Components/MeshRenderer.hpp>

using namespace glm;
using namespace std;
using namespace Assimp;
using namespace Yonai;
using namespace Yonai::IO;
using namespace Yonai::Graphics;
using namespace Yonai::Components;

void Model::Import(string path, vector<unsigned char>& modelData, bool importMaterials)
{
	if (!m_Path.empty())
		// Release previous model
		m_Root = {};

	m_Path = path;
	m_ImportMaterials = importMaterials;

	Load(modelData);
}

void ApplyAssimpTransformation(aiMatrix4x4 transformation, Transform* transform)
{
	aiVector3D scale = {};
	aiQuaternion rotation = {};
	aiVector3D position = {};
	transformation.Decompose(scale, rotation, position);

	transform->SetScale({ scale.x, scale.y, scale.z });
	transform->SetPosition({ position.x, position.y, position.z });
	transform->SetRotation(eulerAngles(quat{ rotation.w, rotation.x, rotation.y, rotation.z }));
}

bool Model::ImportMaterials() { return m_ImportMaterials; }
vector<ResourceID> Model::GetMeshes() { return m_MeshIDs; }

void Model::CreateMeshMaterialPair(const MeshData& data, vector<pair<ResourceID, ResourceID>>& output)
{
	size_t meshCount = data.MeshIDs.size();
	size_t materialCount = data.MaterialIDs.size();
	size_t max = std::max(meshCount, materialCount);
	output.reserve(max);
	for(size_t i = 0; i < max; i++)
		output.emplace_back(make_pair(
			i < meshCount ? m_MeshIDs[data.MeshIDs[i]] : InvalidResourceID,
			i < materialCount ? data.MaterialIDs[i] : InvalidResourceID
		));

	for(const MeshData& child : data.Children)
		CreateMeshMaterialPair(child, output);
}

vector<pair<ResourceID, ResourceID>> Model::GetMeshesAndMaterials()
{
	vector<pair<ResourceID, ResourceID>> output;
	CreateMeshMaterialPair(m_Root, output);
	return output;
} 

void Model::Load(vector<unsigned char>& modelData)
{
	if (m_Path.empty())
		return;

	Importer importer;

	const aiScene* scene = nullptr;
	unsigned int postProcessing = aiProcess_Triangulate | aiProcess_FlipUVs;

	if (modelData.size() == 0)
	{
		spdlog::error("Failed to load model '{}' - Empty data", m_Path);
		return;
	}
	string extension = filesystem::path(m_Path).extension().string();

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

	outputID = Resource::Load<Texture>(texturePath);
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

		if (!m_ImportMaterials)
			continue;

		// Import material
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
#include <Yonai/Scripting/Assembly.hpp>
#include <Yonai/Scripting/ScriptEngine.hpp>
#include <Yonai/Scripting/InternalCalls.hpp>

ADD_MANAGED_METHOD(Model, Load, void, (MonoString* pathRaw, uint64_t* outID, void** outHandle), Yonai.Graphics)
{
	if (*outID == InvalidResourceID)
		*outID = ResourceID(); // Assign resource ID

	char* path = mono_string_to_utf8(pathRaw);
	Resource::Load<Model>(*outID, path);
	*outHandle = Resource::Get<Model>(*outID);
	mono_free(path);
}

ADD_MANAGED_METHOD(Model, Import, void, (void* handle, MonoString* filepath, MonoArray* modelData, bool importMaterials), Yonai.Graphics)
{
	vector<unsigned char> data;
	data.resize(mono_array_length(modelData));
	for (size_t i = 0; i < data.size(); i++)
		data[i] = mono_array_get(modelData, unsigned char, i);
	((Model*)handle)->Import(mono_string_to_utf8(filepath), data, importMaterials);
}

ADD_MANAGED_METHOD(Model, GetMeshes, void, (void* handle, MonoArray** outMeshIDs, MonoArray** outMaterialIDs), Yonai.Graphics)
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
