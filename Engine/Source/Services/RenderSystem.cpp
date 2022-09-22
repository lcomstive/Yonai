#include <vector>
#include <glm/glm.hpp>
#include <AquaEngine/Time.hpp>
#include <AquaEngine/World.hpp>
#include <glm/gtx/quaternion.hpp>
#include <AquaEngine/Resource.hpp>
#include <AquaEngine/Graphics/Texture.hpp>
#include <AquaEngine/Graphics/Material.hpp>
#include <AquaEngine/Systems/SceneSystem.hpp>
#include <AquaEngine/Systems/RenderSystem.hpp>
#include <AquaEngine/Graphics/Pipelines/Forward.hpp>
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
	m_SceneSystem = SystemManager::Get<SceneSystem>();
	if (!m_SceneSystem)
	{
		// No scene system, disable this system
		Enable(false);
		return;
	}
}

void RenderSystem::Draw()
{
	if (!m_SceneSystem || !m_SceneSystem->IsEnabled())
		return;

	if (!m_Pipeline)
		SetPipeline<ForwardRenderPipeline>();

	auto scenes = m_SceneSystem->GetActiveScenes();
	for (auto& scene : scenes)
	{
		auto cameras = scene->GetComponents<Camera>();
		for (auto& camera : cameras)
			m_Pipeline->Draw(camera);
	}
}
