#include <imgui.h>
#include <Views/Stats.hpp>
#include <AquaEngine/Input.hpp>
#include <AquaEngine/Time.hpp>

using namespace AquaEngine;
using namespace AquaEditor;

void StatsView::Draw()
{
	ImGui::Begin("Stats");
	ImGui::Text("FPS: %.1f", Time::FPS());
	ImGui::Text("Delta Time: %.2f ms", Time::DeltaTime() * 1000.0f);

	bool inputEnabled = Input::IsEnabled();
	ImGui::Checkbox("Input enabled", &inputEnabled);
	ImGui::End();
}