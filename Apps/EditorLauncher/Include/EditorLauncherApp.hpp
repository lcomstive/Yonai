#pragma once
#include <string>
#include <typeinfo>
#include <typeindex>
#include <filesystem>
#include <unordered_map>
#include <rapidjson/document.h>
#include <AquaEngine/Application.hpp>

namespace AquaEditorLauncher
{
	struct ProjectInfo
	{
		std::string Name;
		std::string Path;
	};

	class EditorLauncherApp : public AquaEngine::WindowedApplication
	{
		rapidjson::Document m_Settings;

		void ReadSettings();
		void WriteSettings();

	protected:
		void Setup() override;
		void OnDraw() override;

	private:
		void AddProjectPrompt();
		bool LaunchEditor(ProjectInfo& project);
		void RemoveProject(ProjectInfo& project);

		void AddNewProject(std::filesystem::path projectDir);
		void AddExistingProject(std::filesystem::path projectInfoPath);
	};
}