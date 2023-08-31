#pragma once
#include <vector>
#include <Yonai/Graphics/Mesh.hpp>
#include <Yonai/Components/Camera.hpp>
#include <Yonai/Graphics/Framebuffer.hpp>
#include <Yonai/Graphics/RenderPipeline.hpp>
#include <Yonai/Systems/Global/SceneSystem.hpp>

namespace Yonai::Graphics::Pipelines
{
	class ForwardRenderPipeline : public RenderPipeline
	{
		Mesh* m_QuadMesh = nullptr;
		Framebuffer* m_Framebuffer = nullptr;
		Systems::SceneSystem* m_SceneSystem = nullptr;
		
	protected:
		/// <summary>
		/// Draws & lights all meshes
		/// </summary>
		void ForwardPass(Components::Camera* camera);

		virtual void OnResized(glm::ivec2 resolution) override;

	public:
		YonaiAPI ForwardRenderPipeline();
		YonaiAPI ~ForwardRenderPipeline();

		YonaiAPI void Draw(Components::Camera* camera) override;

		YonaiAPI Framebuffer* GetOutput() override;
	};
}
