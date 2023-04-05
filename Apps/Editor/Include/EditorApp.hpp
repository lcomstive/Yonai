#pragma once
#include <string>
#include <typeinfo>
#include <typeindex>
#include <filesystem>
#include <unordered_map>
#include <ProjectInfo.hpp>
#include <AquaEngine/World.hpp>
#include <AquaEngine/Application.hpp>
#include <AquaEngine/Components/Camera.hpp>
#include <AquaEngine/Graphics/RenderTexture.hpp>
#include <AquaEngine/Scripting/ScriptEngine.hpp>
#include <AquaEngine/Systems/Global/RenderSystem.hpp>

namespace AquaEditor
{
	class EditorApp : public AquaEngine::Application
	{
		ProjectInfo m_ProjectInfo = {};
		std::filesystem::path m_ProjectPath;
		AquaEngine::Systems::RenderSystem* m_RenderSystem = nullptr;

		void Draw();
		void DrawUI();

		void InitialiseScripting();

		/// <summary>
		/// Reads project information from '/Project/project.json'.
		/// Stores data in m_ProjectInfo.
		/// </summary>
		void LoadProject();

		/// <summary>
		/// Loads editor script and loads the initial editor service
		/// </summary>
		void LaunchEditorService();

	protected:
		void Setup() override;
		void OnUpdate() override;
		void Cleanup() override;
	};
}
