#pragma once
#include <string>
#include <AquaEngine/World.hpp>
#include <AquaEngine/Application.hpp>
#include <AquaEngine/Components/Camera.hpp>
#include <AquaEngine/Systems/Global/RenderSystem.hpp>
#include <AquaEngine/Graphics/RenderTexture.hpp>

namespace AquaEditor
{
	class EditorApp : public AquaEngine::WindowedApplication
	{
		std::string m_ProjectPath = "";
		AquaEngine::Systems::RenderSystem* m_RenderSystem = nullptr;

		AquaEngine::World* m_CurrentScene = nullptr;
		AquaEngine::Components::Camera* m_EditorCamera = nullptr;

		void DrawUI();
		void LoadScene();

	protected:
		void Setup() override;
		void OnDraw() override;
	};
}