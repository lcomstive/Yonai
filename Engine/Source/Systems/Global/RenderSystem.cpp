#include <vector>
#include <glm/glm.hpp>
#include <AquaEngine/Time.hpp>
#include <AquaEngine/World.hpp>
#include <glm/gtx/quaternion.hpp>
#include <AquaEngine/Resource.hpp>
#include <AquaEngine/Graphics/Texture.hpp>
#include <AquaEngine/Graphics/Material.hpp>
#include <AquaEngine/Graphics/Pipelines/Forward.hpp>
#include <AquaEngine/Systems/Global/SceneSystem.hpp>
#include <AquaEngine/Systems/Global/RenderSystem.hpp>
#include <AquaEngine/Window.hpp>

// Components //
#include <AquaEngine/Components/Camera.hpp>
#include <AquaEngine/Components/Transform.hpp>
#include <AquaEngine/Components/MeshRenderer.hpp>
#include <AquaEngine/Components/SpriteRenderer.hpp>

using namespace std;
using namespace glm;
using namespace AquaEngine;
using namespace AquaEngine::Systems;
using namespace AquaEngine::Graphics;
using namespace AquaEngine::Components;
using namespace AquaEngine::Graphics::Pipelines;

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
		auto cameras = scene->GetComponents<Camera>();
		for (auto camera : cameras)
			Draw(camera);
	}
}

void RenderSystem::Draw(Camera* camera)
{
	GetPipeline()->Draw(camera);
}

#pragma region Internal Calls
#include <AquaEngine/Scripting/InternalCalls.hpp>

ADD_MANAGED_METHOD(Renderer, GetPipeline, void*, (), AquaEngine.Graphics)
{
	RenderSystem* renderSystem = SystemManager::Global()->Get<RenderSystem>();
	RenderPipeline* pipeline = renderSystem->GetPipeline();
	return pipeline;
}

ADD_MANAGED_METHOD(Renderer, Draw, void, (), AquaEngine.Graphics)
{ SystemManager::Global()->Get<RenderSystem>()->Draw(); }

ADD_MANAGED_METHOD(Renderer, DrawFromCamera, void, (unsigned int worldID, unsigned int entityID), AquaEngine.Graphics)
{
	Camera* camera = World::GetWorld(worldID)->GetEntity(entityID).GetComponent<Camera>();
	SystemManager::Global()->Get<RenderSystem>()->Draw(camera);
}

#pragma endregion