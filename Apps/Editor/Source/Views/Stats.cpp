#include <imgui.h>
#include <Views/Stats.hpp>
#include <AquaEngine/Time.hpp>
#include <AquaEngine/Input.hpp>
#include <AquaEngine/Window.hpp>

using namespace AquaEngine;
using namespace AquaEditor;

void StatsView::Open() { m_IsOpen = true; }
void StatsView::Close() { m_IsOpen = false; }

void StatsView::Draw()
{
	if (!m_IsOpen)
		return;

	ImGui::Begin("Stats", &m_IsOpen);
	ImGui::Text("FPS: %.1f", Time::FPS());
	ImGui::Text("Delta Time: %.2f ms", Time::DeltaTime() * 1000.0f);

	bool vsync = Window::GetVSync();
	ImGui::Checkbox("VSync", &vsync);
	Window::SetVSync(vsync);

	ImGui::End();		
}