#pragma once
#include <string>
#include <typeinfo>
#include <typeindex>
#include <filesystem>
#include <unordered_map>
#include <Yonai/World.hpp>
#include <Yonai/Application.hpp>
#include <Yonai/SystemManager.hpp>
#include <YonaiEditor/ProjectInfo.hpp>
#include <Yonai/Components/Camera.hpp>
#include <Yonai/Graphics/RenderTexture.hpp>
#include <Yonai/Scripting/ScriptEngine.hpp>

namespace YonaiEditor
{
	class EditorApp : public Yonai::Application
	{
		static Yonai::SystemManager m_SystemManager;

		void Draw();

		void InitialiseScripting();

		/// <summary>
		/// Loads editor script and loads the initial editor service
		/// </summary>
		void LaunchEditorService();

	public:
		EditorApp();
		EditorApp(int argc, char** argv);

		static Yonai::SystemManager& GetSystemManager();

	protected:
		void Setup() override;
		void OnUpdate() override;
		void Cleanup() override;
	};
}
