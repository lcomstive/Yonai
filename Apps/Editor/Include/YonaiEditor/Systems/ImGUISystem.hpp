#pragma once
#include <string>
#include <imgui.h>
#include <Yonai/Systems/System.hpp>

namespace YonaiEditor::Systems
{
	struct ImGUISystem : public Yonai::Systems::System
	{
		ImGuiIO* m_IO = nullptr;
		std::string m_IniFilepath = "";
		ImGuiContext* m_Context = nullptr;

		void StartFrame();
		void EndFrame();

	public:
		ImGUISystem();

		void OnEnabled() override;
		void OnDisabled() override;

		void Draw() override;

		ImGuiIO* GetIO();
		ImGuiContext* GetContext();
	};
}