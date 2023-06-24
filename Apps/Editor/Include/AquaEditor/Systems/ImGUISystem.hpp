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
		AquaAPI ImGUISystem();

		AquaAPI void OnEnabled() override;
		AquaAPI void OnDisabled() override;

		AquaAPI void Draw() override;

		AquaAPI ImGuiIO* GetIO();
		AquaAPI ImGuiContext* GetContext();
	};
}