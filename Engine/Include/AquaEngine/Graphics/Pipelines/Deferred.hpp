#pragma once
#include <vector>
#include <AquaEngine/Graphics/Mesh.hpp>
#include <AquaEngine/Graphics/Material.hpp>
#include <AquaEngine/Components/Transform.hpp>
#include <AquaEngine/Graphics/RenderPipeline.hpp>
#include <AquaEngine/Components/MeshRenderer.hpp>

namespace AquaEngine::Graphics::Pipelines
{
	class DeferredRenderPipeline : public RenderPipeline
	{
		Mesh* m_QuadMesh;
		glm::ivec2 m_CurrentResolution;
		Components::Camera* m_CurrentCamera;
		std::vector<Components::MeshRenderer*> m_CurrentMeshes;

		Framebuffer *m_MeshFB, *m_LightingFB, *m_ForwardFB;
		Shader *m_LightingShader;

	protected:
		/// <summary>
		/// Draws a mesh to the current framebuffer
		/// </summary>
		void DrawMesh(Components::Transform* transform, Mesh* mesh, Shader* shader);

		/// <summary>
		/// Draws opaque meshes
		/// </summary>
		void MeshPass();

		/// <summary>
		/// Draws & lights transparent meshes, as well as sprites
		/// </summary>
		void ForwardPass();

		/// <summary>
		/// Lights opaque meshes
		/// </summary>
		void LightingPass();

	public:
		AquaAPI DeferredRenderPipeline();
		AquaAPI ~DeferredRenderPipeline();

		AquaAPI Framebuffer* GetOutput() override;
		AquaAPI void Draw(Components::Camera* camera) override;
	};
}