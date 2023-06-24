#pragma once
#include <string>
#include <typeinfo>
#include <typeindex>
#include <filesystem>
#include <unordered_map>
#include <AquaEngine/World.hpp>
#include <AquaEngine/Application.hpp>
#include <AquaEditor/ProjectInfo.hpp>
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

		void InitialiseMounts();
		void InitialiseScripting();

		/// <summary>
		/// Loads editor script and loads the initial editor service
		/// </summary>
		void LaunchEditorService();

	public:
		EditorApp() : AquaEngine::Application() {}
		EditorApp(int argc, char** argv) : AquaEngine::Application(argc, argv) {}

	protected:
		void Setup() override;
		void OnUpdate() override;
		void Cleanup() override;
	};
}
