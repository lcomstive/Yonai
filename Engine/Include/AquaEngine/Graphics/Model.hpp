#pragma once
#include <string>
#include <vector>
#include <assimp/scene.h>
#include <AquaEngine/World.hpp>
#include <AquaEngine/Application.hpp>
#include <AquaEngine/Graphics/Mesh.hpp>
#include <AquaEngine/Graphics/Material.hpp>

namespace AquaEngine::Graphics
{
	class Model
	{
		struct MeshData
		{
			std::string Name = "";
			aiMatrix4x4 Transformation;
			std::vector<MeshData> Children;
			std::vector<unsigned int> MeshIDs; // Index of Model::m_MeshIDs
			std::vector<ResourceID> MaterialIDs;
		};

		MeshData m_Root;
		std::string m_Path;
		bool m_ImportMaterials;
		std::vector<ResourceID> m_MeshIDs;

		void Load();
		ResourceID CreateMaterial(aiMaterial* material);
		void CreateEntity(MeshData& mesh, Entity* parent);
		ResourceID ProcessMesh(aiMesh* mesh, const aiScene* scene);
		void ProcessNode(aiNode* node, MeshData* parent, const aiScene* scene);
		void CreateMeshMaterialPair(const MeshData& data, std::vector<std::pair<ResourceID, ResourceID>>& output);

	public:
		Model();
		Model(std::string& path);

		void Import(const char* path, bool importMaterials = true);
		void Import(std::string path, bool importMaterials = true);

		/// <summary>
		/// When true, materials are imported
		/// </summary>
		bool ImportMaterials();

		std::vector<ResourceID> GetMeshes();

		/// <returns>Meshes and their generated materials</returns>
		std::vector<std::pair<ResourceID, ResourceID>> GetMeshesAndMaterials();
	};
}
