#pragma once
#include <Yonai/ResourceID.hpp>
#include <Yonai/Components/Camera.hpp>
#include <Yonai/Graphics/Framebuffer.hpp>

namespace Yonai::Graphics
{
	class RenderPipeline
	{
		glm::ivec2 m_Resolution = { 0, 0 };

	protected:
		virtual void OnResized(glm::ivec2 resolution) {}

	public:
		virtual ~RenderPipeline() { }
	
		/// <summary>
		/// Renders a scene in to camera's RenderTexture, or to screen if none
		/// </summary>
		YonaiAPI virtual void Draw(Components::Camera* camera) = 0;

		/// <summary>
		/// The framebuffer with the final result
		/// </summary>
		YonaiAPI virtual Framebuffer* GetOutput() = 0;

		YonaiAPI glm::ivec2 GetResolution();
		YonaiAPI void SetResolution(glm::ivec2 resolution);
	};
}