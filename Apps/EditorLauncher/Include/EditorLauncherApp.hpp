#pragma once
#undef max // Removes max macro if included by Windows' minwindef.h

#include <string>
#include <typeinfo>
#include <typeindex>
#include <filesystem>
#include <unordered_map>
#include <AquaEngine/Application.hpp>

namespace AquaEditorLauncher
{	
	class EditorLauncherApp : public AquaEngine::WindowedApplication
	{
	public:
		EditorLauncherApp() : WindowedApplication() { }
		EditorLauncherApp(int argc, char** argv) : WindowedApplication(argc, argv) { }

	protected:
		void Setup() override;
		void OnDraw() override;

	private:
		void AddProjectPrompt();
		bool LaunchEditor();

		void AddNewProject(std::filesystem::path projectDir);
		void AddExistingProject(std::filesystem::path projectInfoPath);
		void CreateCSharpProject(std::filesystem::path projectDir, std::string projectName);
	};
}