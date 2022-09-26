#pragma once
#include <AquaEngine/ResourceID.hpp>
#include <AquaEngine/Components/Camera.hpp>
#include <AquaEngine/Graphics/Framebuffer.hpp>

namespace AquaEngine::Graphics
{
	class RenderPipeline
	{
		glm::ivec2 m_Resolution = { 0, 0 };

	protected:
		void DrawSkybox(ResourceID skyboxTextureID);

		glm::ivec2 GetResolution();
		virtual void OnResized(glm::ivec2 resolution) {}

	public:
		virtual ~RenderPipeline() { }
	
		/// <summary>
		/// Renders a scene in to camera's RenderTexture, or to screen if none
		/// </summary>
		AquaAPI virtual void Draw(Components::Camera* camera) = 0;

		/// <summary>
		/// The framebuffer with the final result
		/// </summary>
		AquaAPI virtual Framebuffer* GetOutput() = 0;

		AquaAPI void SetResolution(glm::ivec2 resolution);
	};
}