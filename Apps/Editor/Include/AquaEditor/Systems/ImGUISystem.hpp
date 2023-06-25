#pragma once
#include <imgui.h>
#include <AquaEngine/Systems/System.hpp>

namespace AquaEditor::Systems
{
	struct ImGUISystem : public AquaEngine::Systems::System
	{
		ImGuiIO* m_IO = nullptr;
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