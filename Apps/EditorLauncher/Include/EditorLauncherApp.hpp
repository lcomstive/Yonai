#pragma once
#include <string>
#include <typeinfo>
#include <typeindex>
#include <unordered_map>
#include <AquaEngine/Application.hpp>

namespace AquaEditorLauncher
{
	class EditorLauncherApp : public AquaEngine::WindowedApplication
	{
		void DrawUI();

	protected:
		void Setup() override;
		void OnDraw() override;

	private:
		bool LaunchEditor(std::string& projectPath);
	};
}