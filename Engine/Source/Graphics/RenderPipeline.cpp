#include <Yonai/Time.hpp>
#include <Yonai/Resource.hpp>
#include <Yonai/Graphics/Mesh.hpp>
#include <Yonai/Graphics/Texture.hpp>
#include <Yonai/Components/Camera.hpp>
#include <Yonai/Graphics/RenderPipeline.hpp>
#include <Yonai/Window.hpp>

using namespace glm;
using namespace std;
using namespace Yonai;
using namespace Yonai::Graphics;
using namespace Yonai::Components;

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
#include <Yonai/Scripting/InternalCalls.hpp>

ADD_MANAGED_METHOD(NativeRenderPipeline, Draw, void, (void* handle, void* cameraHandle), Yonai.Graphics.Pipelines)
{
	if(cameraHandle)
		((RenderPipeline*)handle)->Draw((Camera*)cameraHandle);
}

ADD_MANAGED_METHOD(NativeRenderPipeline, SetResolution, void, (void* handle, glm::ivec2* resolution), Yonai.Graphics.Pipelines)
{ ((RenderPipeline*)handle)->SetResolution(*resolution); }

ADD_MANAGED_METHOD(NativeRenderPipeline, GetResolution, void, (void* handle, glm::ivec2* resolution), Yonai.Graphics.Pipelines)
{ *resolution = ((RenderPipeline*)handle)->GetResolution(); }

ADD_MANAGED_METHOD(NativeRenderPipeline, GetOutput, void*, (void* handle), Yonai.Graphics.Pipelines)
{
	Framebuffer* fb = ((RenderPipeline*)handle)->GetOutput();
	return fb;
}
#pragma endregion