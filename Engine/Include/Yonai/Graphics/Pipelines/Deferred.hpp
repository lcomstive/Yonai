#pragma once
#include <vector>
#include <Yonai/Graphics/Mesh.hpp>
#include <Yonai/Graphics/Material.hpp>
#include <Yonai/Components/Transform.hpp>
#include <Yonai/Graphics/RenderPipeline.hpp>
#include <Yonai/Components/MeshRenderer.hpp>

namespace Yonai::Graphics::Pipelines
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
		YonaiAPI DeferredRenderPipeline();
		YonaiAPI ~DeferredRenderPipeline();

		YonaiAPI Framebuffer* GetOutput() override;
		YonaiAPI void Draw(Components::Camera* camera) override;
	};
}