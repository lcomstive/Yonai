#include <imgui.h>
#include <Views/Viewport.hpp>
#include <AquaEngine/Input.hpp>
#include <AquaEngine/SystemManager.hpp>

using namespace glm;
using namespace AquaEditor;
using namespace AquaEngine;
using namespace AquaEngine::Systems;
using namespace AquaEngine::Components;

ViewportView::ViewportView()
{
	m_RenderSystem = SystemManager::Global()->Get<RenderSystem>();

	m_EditorWorld = new World("Viewport");

	Entity cameraEntity = m_EditorWorld->CreateEntity();
	m_EditorCamera = cameraEntity.AddComponent<Camera>();
	m_EditorCameraControls = cameraEntity.AddComponent<FPSCamera>();
	Transform* transform = cameraEntity.AddComponent<Transform>();
	transform->Position = { 0, 0, -10 };

	m_CameraControlSystem = new CameraControlSystem();

	// Temp //
	m_EditorCamera->SetMainCamera();
}

ViewportView::~ViewportView()
{
	delete m_CameraControlSystem;
}

void ViewportView::Update()
{
	m_CameraControlSystem->Update(m_EditorCameraControls);
}

void ViewportView::Draw()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
	ImGui::Begin("Viewport", nullptr);
		
	auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
	auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
	auto viewportOffset = ImGui::GetWindowPos();

	vec2 m_ViewportBounds[2] = { { 0, 0 }, { 0, 0 } };
	m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
	m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

	bool m_ViewportFocused = ImGui::IsWindowFocused();
	bool m_ViewportHovered = ImGui::IsWindowHovered();

	Input::Enable(m_ViewportFocused);

	ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
	vec2 m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

	if(m_EditorCamera)
	{
		m_RenderSystem->GetPipeline()->SetResolution(m_ViewportSize);
		m_RenderSystem->Draw(m_EditorCamera);

		ImGui::Image(reinterpret_cast<void*>(m_RenderSystem->GetPipeline()->GetOutput()->GetColourAttachment()->GetID()),
			ImVec2{ m_ViewportSize.x, m_ViewportSize.y },
			ImVec2{ 0, 1 },
			ImVec2{ 1, 0 }
		);
	}

	ImGui::End();
	ImGui::PopStyleVar();
}