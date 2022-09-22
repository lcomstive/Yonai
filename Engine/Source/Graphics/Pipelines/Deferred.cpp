#include <AquaEngine/Time.hpp>
#include <AquaEngine/Resource.hpp>
#include <AquaEngine/Graphics/Mesh.hpp>
#include <AquaEngine/Components/Light.hpp>
#include <AquaEngine/Graphics/Material.hpp>
#include <AquaEngine/Components/SpriteRenderer.hpp>
#include <AquaEngine/Graphics/Pipelines/Deferred.hpp>
#include <AquaEngine/Window.hpp>

using namespace glm;
using namespace std;
using namespace AquaEngine;
using namespace AquaEngine::Graphics;
using namespace AquaEngine::Components;
using namespace AquaEngine::Graphics::Pipelines;

void FillLightInfo(Shader* shader, vector<pair<Light*, Transform*>>& lights);

DeferredRenderPipeline::DeferredRenderPipeline() : RenderPipeline()
{
	FramebufferSpec framebufferSpecs = { Window::GetResolution() };

	// Mesh Pass //
	framebufferSpecs.Attachments =
	{
		TextureFormat::RGBA16F,	// Position
		TextureFormat::RGBA16F,	// Normal
		TextureFormat::RGBA8,	// Albedo w/ roughness in alpha channel
		TextureFormat::RGBA8,	// Ambient Occlusion w/ metalness in alpha channel
		TextureFormat::Depth
	};

	m_MeshFB = new Framebuffer(framebufferSpecs);

	// Lighting Pass //
	framebufferSpecs.Attachments =
	{
		TextureFormat::RGBA8,
		TextureFormat::Depth
	};
	m_LightingFB = new Framebuffer(framebufferSpecs);

	m_LightingShader = Resource::LoadPtr<Shader>("Shaders/Deferred/Lighting",
		ShaderStageInfo
		{
			"/Assets/Shaders/Deferred/Lighting.vert",
			"/Assets/Shaders/Deferred/Lighting.frag",
		});

	// Transparent Forward Pass //
	m_ForwardFB = new Framebuffer(framebufferSpecs);

	// Get Quad mesh //
	m_QuadMesh = Resource::Get<Mesh>(Mesh::Quad());
}

DeferredRenderPipeline::~DeferredRenderPipeline()
{
	delete m_MeshFB;
	delete m_ForwardFB;
	delete m_LightingFB;

	delete m_LightingShader;
}

void DeferredRenderPipeline::Draw(Camera* camera)
{
	m_CurrentCamera = camera;
	m_CurrentMeshes = camera->Entity.GetWorld()->GetComponents<MeshRenderer>();
	m_CurrentResolution = camera->RenderTarget ? camera->RenderTarget->GetResolution() : Window::GetResolution();

	// TODO: (Frustum?) Culling

	// Do the render
	MeshPass();
	LightingPass();
	ForwardPass();

	// Release resources
	m_CurrentMeshes.clear();
	m_CurrentCamera = nullptr;
}

void DeferredRenderPipeline::DrawMesh(Transform* transform, Mesh* mesh, Shader* shader)
{
	// Fill shader
	shader->Set("time", Time::SinceLaunch());
	shader->Set("resolution", m_CurrentResolution);

	m_CurrentCamera->FillShader(shader, m_CurrentResolution);
	shader->Set("modelMatrix", transform->GetModelMatrix());

	auto lights = m_CurrentCamera->Entity.GetWorld()->GetComponents<Light, Transform>();
	FillLightInfo(m_LightingShader, lights);

	// Draw mesh
	mesh->Draw();

	// Unbind resources
	shader->Unbind();
}

void DeferredRenderPipeline::MeshPass()
{
	glEnable(GL_DEPTH_TEST);
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_MeshFB->Bind();

	for (MeshRenderer* renderer : m_CurrentMeshes)
	{
		if (renderer->Mesh == InvalidResourceID ||
			renderer->Material == InvalidResourceID)
			continue; // Invalid parameters

#pragma region Getting pointerrsss
		Mesh* mesh = Resource::Get<Mesh>(renderer->Mesh);
		Material* material = Resource::Get<Material>(renderer->Material);
		Transform* transform = renderer->Entity.GetComponent<Transform>();
		if (!mesh || material)
			continue; // Invalid parameters

		if (!mesh || !material || !transform ||
			material->Shader == InvalidResourceID ||
			material->Transparent)
			continue; // Invalid resource(s), or not opaque
#pragma endregion

		DrawMesh(transform, mesh, material->PrepareShader());
	}
	m_MeshFB->Unbind();
}

void DeferredRenderPipeline::LightingPass()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_LightingFB->Bind();
	m_LightingShader->Bind();

	// FILL G-BUFFER MAPS //
	// Position
	m_MeshFB->GetColourAttachment()->Bind();
	m_LightingShader->Set("inputPosition", 0);

	// Normals
	m_MeshFB->GetColourAttachment(1)->Bind(1);
	m_LightingShader->Set("inputNormal", 1);

	// Albedo + Roughness
	m_MeshFB->GetColourAttachment(2)->Bind(2);
	m_LightingShader->Set("inputAlbedoRoughness", 2);

	// Ambient Occlusion + Metalness
	m_MeshFB->GetColourAttachment(3)->Bind(3);
	m_LightingShader->Set("inputAmbientMetalness", 3);

	// Depth
	m_MeshFB->GetDepthAttachment()->Bind(4);
	m_LightingShader->Set("inputDepth", 4);

	// FILL LIGHT DATA //
	auto lights = m_CurrentCamera->Entity.GetWorld()->GetComponents<Light, Transform>();
	FillLightInfo(m_LightingShader, lights);

	// DRAW FULLSCREEN QUAD //
	Resource::Get<Mesh>(Mesh::Quad())->Draw();

	// Unbind resources
	m_LightingFB->Unbind();
}

void DeferredRenderPipeline::ForwardPass()
{
	m_ForwardFB->Bind();

	// Blit the colour info from the lighting pass
	m_LightingFB->BlitTo(m_ForwardFB, GL_COLOR_BUFFER_BIT);

	// Blit the depth info from the mesh pass
	m_MeshFB->BlitTo(m_ForwardFB, GL_DEPTH_BUFFER_BIT);

	// Draw transparent objects
	for (MeshRenderer* renderer : m_CurrentMeshes)
	{
		if (renderer->Mesh == InvalidResourceID ||
			renderer->Material == InvalidResourceID)
			continue; // Invalid parameters

#pragma region Getting pointerrsss
		Mesh* mesh = Resource::Get<Mesh>(renderer->Mesh);
		Material* material = Resource::Get<Material>(renderer->Material);
		Transform* transform = renderer->Entity.GetComponent<Transform>();

		if (material->Shader == InvalidResourceID ||
			!material->Transparent)
			continue; // Invalid shader, or not opaque
#pragma endregion

		DrawMesh(transform, mesh, material->PrepareShader());
	}

	// Draw sprites
	vector<SpriteRenderer*> sprites = m_CurrentCamera->Entity.GetWorld()->GetComponents<SpriteRenderer>();
	for (SpriteRenderer* renderer : sprites)
	{
		if (renderer->Shader == InvalidResourceID)
			continue; // Invalid parameters

		Shader* shader = Resource::Get<Shader>(renderer->Shader);
		Transform* transform = renderer->Entity.GetComponent<Transform>();

		if (!shader || !transform)
			continue; // Invalid parameters

		DrawMesh(transform, m_QuadMesh, shader);
	}

	// Draw skybox
	// DrawSkybox();

	m_ForwardFB->Unbind();

	if (m_CurrentCamera->RenderTarget)
		// Copy to camera's render target
		m_ForwardFB->CopyAttachmentTo(m_CurrentCamera->RenderTarget);
	else
		// Blit to default framebuffer (screen)
		m_ForwardFB->BlitTo(nullptr);
}

const unsigned int MaxLights = 4;
void FillLightInfo(Shader* shader, vector<pair<Light*, Transform*>>& lights)
{
	unsigned int lightIndex = 0;
	unsigned int lightCount = std::min((unsigned int)lights.size(), MaxLights);

	shader->Set("lightCount", (int)lightCount);
	for (auto& lightPair : lights)
	{
		shader->Set("lights[" + to_string(lightIndex) + "].Colour", lightPair.first->Colour);
		shader->Set("lights[" + to_string(lightIndex) + "].Radius", lightPair.first->Radius);
		shader->Set("lights[" + to_string(lightIndex) + "].Position", lightPair.second->Position);

		if (++lightIndex >= MaxLights)
			break;
	}
}