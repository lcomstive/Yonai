#pragma once
#include <mono/jit/jit.h>
#include <Yonai/Graphics/RenderPipeline.hpp>

namespace Yonai::Graphics::Pipelines
{
	class ScriptedRenderPipeline : public RenderPipeline
	{
		virtual void OnResized(glm::ivec2 resolution) override;

	public:
		YonaiAPI ScriptedRenderPipeline();
		YonaiAPI ~ScriptedRenderPipeline();

		YonaiAPI void Draw(Components::Camera* camera) override;

		YonaiAPI Framebuffer* GetOutput() override;
	};

}