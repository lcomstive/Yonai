#include <Yonai/Time.hpp>
#include <Yonai/Resource.hpp>
#include <Yonai/Graphics/Mesh.hpp>
#include <Yonai/SystemManager.hpp>
#include <Yonai/Graphics/Texture.hpp>
#include <Yonai/Components/Light.hpp>
#include <Yonai/Graphics/Material.hpp>
#include <Yonai/Components/Transform.hpp>
#include <Yonai/Components/MeshRenderer.hpp>
#include <Yonai/Components/SpriteRenderer.hpp>
#include <Yonai/Graphics/Pipelines/Forward.hpp>
#include <Yonai/Window.hpp>

using namespace glm;
using namespace std;
using namespace Yonai;
using namespace Yonai::Systems;
using namespace Yonai::Graphics;
using namespace Yonai::Components;
using namespace Yonai::Graphics::Pipelines;

ForwardRenderPipeline::ForwardRenderPipeline() : RenderPipeline()
{
	glEnable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	FramebufferSpec framebufferSpecs = { Window::GetResolution() };
	framebufferSpecs.Attachments =
	{
		TextureFormat::RGBA8,
		TextureFormat::Depth
	};
	
	m_Framebuffer = new Framebuffer(framebufferSpecs);
	m_QuadMesh = Resource::Get<Mesh>(Mesh::Quad());

	m_SceneSystem = SystemManager::Global()->Get<SceneSystem>();
}

ForwardRenderPipeline::~ForwardRenderPipeline()
{
	// Restore 
	glDisable(GL_CULL_FACE);
	delete m_Framebuffer;
}

void ForwardRenderPipeline::Draw(Camera* camera)
{
	ForwardPass(camera);
}

Framebuffer* ForwardRenderPipeline::GetOutput() { return m_Framebuffer; }

void DrawMesh(Mesh* mesh, Shader* shader, Transform* transform, Camera* camera, ivec2 resolution)
{
	// Fill shader
	shader->Set("time", Time::SinceLaunch());
	shader->Set("resolution", resolution);

	camera->FillShader(shader, resolution);
	shader->Set("modelMatrix", transform->GetModelMatrix());

	// Draw mesh
	mesh->Draw();

	// Unbind resources
	shader->Unbind();
}

void ForwardRenderPipeline::OnResized(glm::ivec2 resolution)
{
	m_Framebuffer->SetResolution(resolution);
}

void ForwardRenderPipeline::ForwardPass(Camera* camera)
{
	m_Framebuffer->Bind();

	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	vector<World*>& scenes = m_SceneSystem->GetActiveScenes();

	ivec2 currentResolution = camera->RenderTarget ? camera->RenderTarget->GetResolution() : GetResolution();

	if (currentResolution.x <= 0 || currentResolution.y <= 0)
		return; // Nothing to draw

	glViewport(0, 0, currentResolution.x, currentResolution.y);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	for(World* scene : scenes)
	{
		vector<MeshRenderer*> meshes = scene->GetComponents<MeshRenderer>();
		
		// Draw objects
		for (MeshRenderer* renderer : meshes)
		{
			if (renderer->Mesh == InvalidResourceID ||
				renderer->Material == InvalidResourceID)
				continue; // Invalid parameters

	#pragma region Getting pointers
			Mesh* mesh = Resource::Get<Mesh>(renderer->Mesh);
			Material* material = Resource::Get<Material>(renderer->Material);
			Transform* transform = renderer->Entity.GetComponent<Transform>();

			if (!mesh || !material || !transform ||
				material->Shader == InvalidResourceID)
				continue; // Invalid resource(s)
	#pragma endregion

			Shader* shader = material->PrepareShader();
			DrawMesh(mesh, shader, transform, camera, currentResolution);
		}

		glDisable(GL_CULL_FACE);

		// Draw sprites
		vector<SpriteRenderer*> sprites = scene->GetComponents<SpriteRenderer>();
		for(SpriteRenderer* renderer : sprites)
		{
			if (renderer->Shader == InvalidResourceID)
				continue; // Invalid parameters

			Shader* shader = Resource::Get<Shader>(renderer->Shader);
			Texture* texture = Resource::Get<Texture>(renderer->Sprite);
			Transform* transform = renderer->Entity.GetComponent<Transform>();

			if (!shader || !transform || !texture)
				continue; // Invalid resource(s)

			shader->Bind();
			texture->Bind();
			shader->Set("inputTexture", 0);
			shader->Set("colour", renderer->Colour);

			DrawMesh(m_QuadMesh, shader, transform, camera, currentResolution);
		}
	}

	// Draw skybox
	// DrawSkybox();

	m_Framebuffer->Unbind();

	if (camera->RenderTarget)
		// Copy to camera's render target
		m_Framebuffer->CopyAttachmentTo(camera->RenderTarget);
}