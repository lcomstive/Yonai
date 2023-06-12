#include <AquaEngine/Time.hpp>
#include <AquaEngine/Resource.hpp>
#include <AquaEngine/Graphics/Mesh.hpp>
#include <AquaEngine/Graphics/Texture.hpp>
#include <AquaEngine/Components/Camera.hpp>
#include <AquaEngine/Graphics/RenderPipeline.hpp>
#include <AquaEngine/Window.hpp>

using namespace glm;
using namespace std;
using namespace AquaEngine;
using namespace AquaEngine::Graphics;
using namespace AquaEngine::Components;

void RenderPipeline::SetResolution(ivec2 resolution)
{
	if(m_Resolution == resolution)
		return; // Same, ignore
		
	// spdlog::trace("Resolution set to {}x{}", resolution.x, resolution.y);
	m_Resolution = resolution;
	OnResized(resolution);
}

ivec2 RenderPipeline::GetResolution() { return m_Resolution; }

#pragma region Internal Calls
#include <AquaEngine/Scripting/InternalCalls.hpp>

ADD_MANAGED_METHOD(NativeRenderPipeline, Draw, void, (void* handle, void* cameraHandle), AquaEngine.Graphics.Pipelines)
{
	if(cameraHandle)
		((RenderPipeline*)handle)->Draw((Camera*)cameraHandle);
}

ADD_MANAGED_METHOD(NativeRenderPipeline, SetResolution, void, (void* handle, glm::ivec2* resolution), AquaEngine.Graphics.Pipelines)
{ ((RenderPipeline*)handle)->SetResolution(*resolution); }

ADD_MANAGED_METHOD(NativeRenderPipeline, GetResolution, void, (void* handle, glm::ivec2* resolution), AquaEngine.Graphics.Pipelines)
{ *resolution = ((RenderPipeline*)handle)->GetResolution(); }

ADD_MANAGED_METHOD(NativeRenderPipeline, GetOutput, void*, (void* handle), AquaEngine.Graphics.Pipelines)
{
	Framebuffer* fb = ((RenderPipeline*)handle)->GetOutput();
	return fb;
}
#pragma endregion