#pragma once
#include <AquaEngine/ResourceID.hpp>
#include <AquaEngine/Components/Camera.hpp>

namespace AquaEngine::Graphics
{
	class RenderPipeline
	{
	protected:
		void DrawSkybox(ResourceID skyboxTextureID);

	public:
		/// <summary>
		/// Renders a scene in to camera's RenderTexture, or to screen if none
		/// </summary>
		AquaAPI virtual void Draw(Components::Camera* camera) = 0;
	};
}