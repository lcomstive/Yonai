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
		
	m_Resolution = resolution;
	OnResized(resolution);
}

ivec2 RenderPipeline::GetResolution() { return m_Resolution; }