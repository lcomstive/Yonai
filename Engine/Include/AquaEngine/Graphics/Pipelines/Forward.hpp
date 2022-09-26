#pragma once
#include <vector>
#include <AquaEngine/Graphics/Mesh.hpp>
#include <AquaEngine/Components/Camera.hpp>
#include <AquaEngine/Graphics/Framebuffer.hpp>
#include <AquaEngine/Graphics/RenderPipeline.hpp>

namespace AquaEngine::Graphics::Pipelines
{
	class ForwardRenderPipeline : public RenderPipeline
	{
		Mesh* m_QuadMesh;
		Framebuffer* m_Framebuffer;
		
	protected:
		/// <summary>
		/// Draws & lights all meshes
		/// </summary>
		void ForwardPass(Components::Camera* camera);

		virtual void OnResized(glm::ivec2 resolution) override;

	public:
		AquaAPI ForwardRenderPipeline();
		AquaAPI ~ForwardRenderPipeline();

		AquaAPI void Draw(Components::Camera* camera) override;

		AquaAPI Framebuffer* GetOutput() override;
	};
}
