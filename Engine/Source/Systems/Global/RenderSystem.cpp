#include <vector>
#include <glm/glm.hpp>
#include <Yonai/Time.hpp>
#include <Yonai/World.hpp>
#include <glm/gtx/quaternion.hpp>
#include <Yonai/Resource.hpp>
#include <Yonai/Graphics/Texture.hpp>
#include <Yonai/Graphics/Material.hpp>
#include <Yonai/Graphics/Pipelines/Forward.hpp>
#include <Yonai/Systems/Global/SceneSystem.hpp>
#include <Yonai/Systems/Global/RenderSystem.hpp>
#include <Yonai/Window.hpp>

// Components //
#include <Yonai/Components/Camera.hpp>
#include <Yonai/Components/Transform.hpp>
#include <Yonai/Components/MeshRenderer.hpp>
#include <Yonai/Components/SpriteRenderer.hpp>

using namespace std;
using namespace glm;
using namespace Yonai;
using namespace Yonai::Systems;
using namespace Yonai::Graphics;
using namespace Yonai::Components;
using namespace Yonai::Graphics::Pipelines;

void RenderSystem::OnEnabled()
{
	m_SceneSystem = SystemManager::Global()->Get<SceneSystem>();
}

RenderPipeline* RenderSystem::GetPipeline()
{
	if (!m_Pipeline)
		SetPipeline<ForwardRenderPipeline>();
	return m_Pipeline;
}

void RenderSystem::Draw()
{
	if (!m_SceneSystem || !m_SceneSystem->IsEnabled())
		return;

	auto scenes = m_SceneSystem->GetActiveScenes();
	for (auto& scene : scenes)
	{
		vector<Camera*> cameras = scene->GetComponents<Camera>();
		for (auto camera : cameras)
			Draw(camera);
	}
}

void RenderSystem::Draw(Camera* camera)
{
	GetPipeline()->Draw(camera);
}

#pragma region Internal Calls
#include <Yonai/Scripting/InternalCalls.hpp>

ADD_MANAGED_METHOD(Renderer, GetPipeline, void*, (), Yonai.Graphics)
{
	RenderSystem* renderSystem = SystemManager::Global()->Get<RenderSystem>();
	RenderPipeline* pipeline = renderSystem->GetPipeline();
	return pipeline;
}

ADD_MANAGED_METHOD(Renderer, Draw, void, (), Yonai.Graphics)
{ SystemManager::Global()->Get<RenderSystem>()->Draw(); }

ADD_MANAGED_METHOD(Renderer, DrawFromCamera, void, (unsigned int worldID, unsigned int entityID), Yonai.Graphics)
{
	Camera* camera = World::GetWorld(worldID)->GetEntity(entityID).GetComponent<Camera>();
	SystemManager::Global()->Get<RenderSystem>()->Draw(camera);
}

#pragma endregion