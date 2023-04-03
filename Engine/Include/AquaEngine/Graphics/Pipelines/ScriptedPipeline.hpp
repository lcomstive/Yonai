#pragma once
#include <mono/jit/jit.h>
#include <AquaEngine/Graphics/RenderPipeline.hpp>

namespace AquaEngine::Graphics::Pipelines
{
	class ScriptedRenderPipeline : public RenderPipeline
	{
		virtual void OnResized(glm::ivec2 resolution) override;

	public:
		AquaAPI ScriptedRenderPipeline();
		AquaAPI ~ScriptedRenderPipeline();

		AquaAPI void Draw(Components::Camera* camera) override;

		AquaAPI Framebuffer* GetOutput() override;
	};

}