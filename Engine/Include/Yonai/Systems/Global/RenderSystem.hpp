#pragma once
#include <Yonai/Graphics/Mesh.hpp>
#include <Yonai/Systems/System.hpp>
#include <Yonai/Graphics/Shader.hpp>
#include <Yonai/Components/Camera.hpp>
#include <Yonai/Components/MeshRenderer.hpp>
#include <Yonai/Graphics/RenderPipeline.hpp>
#include <Yonai/Components/SpriteRenderer.hpp>
#include <Yonai/Systems/Global/SceneSystem.hpp>

namespace Yonai::Systems
{
	struct RenderSystem : public System
	{
	private:
		SceneSystem* m_SceneSystem;
		Graphics::RenderPipeline* m_Pipeline;

	public:
		YonaiAPI void Draw(Components::Camera* camera);

		YonaiAPI void Draw() override;
		YonaiAPI void OnEnabled() override;

		YonaiAPI Graphics::RenderPipeline* GetPipeline();

		template<typename T>
		void SetPipeline()
		{
			if (m_Pipeline)
				delete m_Pipeline;

			m_Pipeline = new T();
		}
	};
}