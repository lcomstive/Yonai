#include <imgui.h>
#include <spdlog/spdlog.h>
#include <AquaEngine/IO/VFS.hpp>
#include <AquaEngine/Window.hpp>
#include <AquaEngine/Systems/Global/ImGUISystem.hpp>
#include <AquaEngine/Systems/Global/ImGUISystemBackend/ImGUIBackend_GLFW3.hpp>
#include <AquaEngine/Systems/Global/ImGUISystemBackend/ImGUIBackend_OpenGL3.hpp>
#include <GLFW/glfw3.h>

using namespace glm;
using namespace AquaEngine;
using namespace AquaEngine::Systems;

ImGUISystem::ImGUISystem()
{
	IMGUI_CHECKVERSION();
	m_Context = ImGui::CreateContext();

	m_IO = &ImGui::GetIO();

	// Activate docking & viewport features
	m_IO->ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable;

	// Scale with system
	vec2 scaling = Window::GetContentScaling();
	m_IO->DisplayFramebufferScale = { scaling.x, scaling.y };
	m_IO->FontGlobalScale = scaling.x;

	// Set colours to dark theme
	ImGui::StyleColorsDark();
}

void ImGUISystem::OnEnabled()
{
	ImGui_ImplGlfw_InitForOpenGL(Window::GetNativeHandle(), true);
	ImGui_ImplOpenGL3_Init();

	std::string fontPath = IO::VFS::GetAbsolutePath("assets://Fonts/OpenSans-Regular.ttf", true);
	m_IO->FontDefault = m_IO->Fonts->AddFontFromFileTTF(fontPath.c_str(), 18);

	StartFrame();
}

void ImGUISystem::OnDisabled()
{
	ImGui::Render();

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
ImGuiContext* ImGUISystem::GetContext() { return m_Context; }