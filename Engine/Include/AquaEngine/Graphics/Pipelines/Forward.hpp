#pragma once
#include <vector>
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

	public:
		ForwardRenderPipeline();
		~ForwardRenderPipeline();

		void Draw(Components::Camera* camera) override;
	};
}
