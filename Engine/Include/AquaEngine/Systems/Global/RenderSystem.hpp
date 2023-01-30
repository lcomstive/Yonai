#pragma once
#include <AquaEngine/Graphics/Mesh.hpp>
#include <AquaEngine/Systems/System.hpp>
#include <AquaEngine/Graphics/Shader.hpp>
#include <AquaEngine/Components/Camera.hpp>
#include <AquaEngine/Components/MeshRenderer.hpp>
#include <AquaEngine/Graphics/RenderPipeline.hpp>
#include <AquaEngine/Components/SpriteRenderer.hpp>
#include <AquaEngine/Systems/Global/SceneSystem.hpp>

namespace AquaEngine::Systems
{
	struct RenderSystem : public System
	{
	private:
		SceneSystem* m_SceneSystem;
		Graphics::RenderPipeline* m_Pipeline;

	public:
		AquaAPI void Draw(Components::Camera* camera);

		AquaAPI void Draw() override;
		AquaAPI void OnEnabled() override;

		AquaAPI Graphics::RenderPipeline* GetPipeline();

		template<typename T>
		void SetPipeline()
		{
			if (m_Pipeline)
				delete m_Pipeline;

			m_Pipeline = new T();
		}
	};
}