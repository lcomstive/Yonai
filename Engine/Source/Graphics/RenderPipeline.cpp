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

void RenderPipeline::DrawSkybox(ResourceID skyboxTextureID)
{
	Shader* skyboxShader = Resource::Get<Shader>(Resource::Load<Shader>("Shaders/Skybox", ShaderStageInfo
		{
			"/Assets/Shaders/Skybox.vs",
			"/Assets/Shaders/Skybox.fs"
		}));
	skyboxShader->Bind();

	Texture* skyboxTexture = Resource::Get<Texture>(skyboxTextureID);
	if (!skyboxTexture)
		return;

	glDepthFunc(GL_LEQUAL);
	
	if (skyboxTexture)
		skyboxTexture->Bind(5);
	skyboxShader->Set("environmentMap", 5);
	
	Camera* camera = Camera::GetMainCamera();
	skyboxShader->Set("view", camera->GetViewMatrix());
	skyboxShader->Set("projection", camera->GetProjectionMatrix(Window::GetResolution()));

	Resource::Get<Mesh>(Mesh::Cube())->Draw();

	skyboxShader->Unbind();
	glDepthFunc(GL_LESS);
}

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

ADD_MANAGED_METHOD(NativeRenderPipeline, Draw, void, (void* handle, uint64_t cameraWorldID, uint64_t cameraEntityID), AquaEngine.Graphics.Pipelines)
{
	Camera* camera = World::GetWorld(cameraWorldID)->GetEntity(cameraEntityID).GetComponent<Camera>();
	if(camera)
		((RenderPipeline*)handle)->Draw(camera);
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