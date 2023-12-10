#pragma once
#include <string>
#include <typeinfo>
#include <typeindex>
#include <filesystem>
#include <unordered_map>
#include <Yonai/World.hpp>
#include <Yonai/Application.hpp>
#include <YonaiEditor/ProjectInfo.hpp>
#include <Yonai/Scripting/ScriptEngine.hpp>

namespace YonaiEditor
{
	class EditorApp : public Yonai::Application
	{
		ProjectInfo m_ProjectInfo = {};
		std::filesystem::path m_ProjectPath;

		void Draw();

		void InitialiseScripting();

		/// <summary>
		/// Loads editor script and loads the initial editor service
		/// </summary>
		void LaunchEditorService();

	public:
		EditorApp() : Yonai::Application() {}
		EditorApp(int argc, char** argv) : Yonai::Application(argc, argv) {}

	protected:
		void Setup() override;
		void OnUpdate() override;
		void Cleanup() override;
	};
}
