#include <imgui.h>
#include <ImGuizmo.h>
#include <spdlog/spdlog.h>
#include <Yonai/Window.hpp>
#include <YonaiEditor/Systems/ImGUISystem.hpp>
#include <YonaiEditor/Systems/ImGUISystemBackend/ImGUIBackend_GLFW3.hpp>
#include <YonaiEditor/Systems/ImGUISystemBackend/ImGUIBackend_OpenGL3.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

using namespace glm;
using namespace Yonai;
using namespace Yonai::Systems;
using namespace YonaiEditor::Systems;

ImGUISystem::ImGUISystem()
{
	IMGUI_CHECKVERSION();
	m_Context = ImGui::CreateContext();
	ImGui::SetCurrentContext(m_Context);

	m_IO = &ImGui::GetIO();
	m_IO->IniFilename = nullptr; // Disable automatic saving and loading
	m_IO->ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_ViewportsEnable;

	// Scale with system
	vec2 scaling = Window::GetContentScaling();
	m_IO->DisplayFramebufferScale = { scaling.x, scaling.y };
	m_IO->FontGlobalScale = scaling.x;

	// Set colours to dark theme
	ImGui::StyleColorsDark();
}

static bool imguiInitialised = false;
void ImGUISystem::OnEnabled()
{
	if(!imguiInitialised)
	{
		ImGui_ImplGlfw_InitForOpenGL(Window::GetNativeHandle(), true);
		ImGui_ImplOpenGL3_Init("#version 410 core");
		StartFrame();

		imguiInitialised = true;
	}

	/*
	std::string fontPath = IO::VFS::GetAbsolutePath("assets://Fonts/OpenSans-Regular.ttf", true);
	m_IO->FontDefault = m_IO->Fonts->AddFontFromFileTTF(fontPath.c_str(), 18);
	*/

	ImGuizmo::SetImGuiContext(ImGui::GetCurrentContext());

	ImGui::LoadIniSettingsFromDisk(m_IniFilepath.c_str());

	spdlog::trace("Enabled ImGUI system");
}

void ImGUISystem::OnDisabled()
{
	spdlog::trace("Disabling ImGUI system");

	ImGui::SaveIniSettingsToDisk(m_IniFilepath.c_str());
	
	ImGui::Render();

	if(imguiInitialised)
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	imguiInitialised = false;
	m_IO = nullptr;
}

void ImGUISystem::Draw()
{
#if !defined(YONAI_PLATFORM_APPLE)
	glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "ImGUI");
#endif

	EndFrame();
	StartFrame();

#if !defined(YONAI_PLATFORM_APPLE)
	glPopDebugGroup();
#endif
}

void ImGUISystem::StartFrame()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGuizmo::BeginFrame();
}

void ImGUISystem::EndFrame()
{
	ImGui::EndFrame();
	ImGui::Render();

	ivec2 resolution = Window::GetFramebufferResolution();
	glViewport(0, 0, resolution.x, resolution.y);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

#pragma region Viewport support
	if (m_IO->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		GLFWwindow* handle = Window::GetNativeHandle();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(handle);
	}
#pragma endregion
}

ImGuiIO* ImGUISystem::GetIO() { return m_IO; }
ImGuiContext* ImGUISystem::GetContext() { return m_Context; }