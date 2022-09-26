#include <imgui.h>
#include <spdlog/spdlog.h>
#include <AquaEngine/Window.hpp>
#include <AquaEngine/Systems/Global/ImGUISystem.hpp>
#include <AquaEngine/Systems/Global/ImGUISystemBackend/ImGUIBackend_GLFW3.hpp>
#include <AquaEngine/Systems/Global/ImGUISystemBackend/ImGUIBackend_OpenGL3.hpp>
#include <GLFW/glfw3.h>

using namespace glm;
using namespace AquaEngine;
using namespace AquaEngine::Systems;

void ImGUISystem::OnEnabled()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	m_IO = &ImGui::GetIO();
	m_IO->ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable;

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(Window::GetNativeHandle(), true);
	ImGui_ImplOpenGL3_Init();

	StartFrame();
}

void ImGUISystem::OnDisabled()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	m_IO = nullptr;
}

void ImGUISystem::Draw()
{
	EndFrame();
	StartFrame();

	// ImGui::DockSpace();
}

void ImGUISystem::StartFrame()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void ImGUISystem::EndFrame()
{
	ImGui::Render();

	ivec2 resolution = Window::GetFramebufferResolution();
	glViewport(0, 0, resolution.x, resolution.y);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

ImGuiIO* ImGUISystem::GetIO() { return m_IO; }