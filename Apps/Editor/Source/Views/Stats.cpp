#include <imgui.h>
#include <Views/Stats.hpp>
#include <AquaEngine/Time.hpp>
#include <AquaEngine/Input.hpp>
#include <AquaEngine/World.hpp>
#include <AquaEngine/Window.hpp>
#include <AquaEngine/Systems/Global/SceneSystem.hpp>

using namespace std;
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

	vector<World*> currentWorlds = SystemManager::Global()->Get<Systems::SceneSystem>()->GetActiveScenes();

	if(currentWorlds.size() > 1)
	{
		ImGui::Text("Worlds: [%lu]", currentWorlds.size());
		ImGui::Indent();
		for(World* world : currentWorlds)
			ImGui::Text("World: %s [%i] [%i entities]", world->Name().c_str(), world->ID(), world->EntityCount());
		ImGui::Unindent();
	}
	else if(currentWorlds.size() == 1)
		ImGui::Text("World: %s [%i] [%i entities]", currentWorlds[0]->Name().c_str(), currentWorlds[0]->ID(), currentWorlds[0]->EntityCount());
	else
		ImGui::Text("World: <none>");

	bool vsync = Window::GetVSync();
	ImGui::Checkbox("VSync", &vsync);
	Window::SetVSync(vsync);

	ImGui::End();		
}