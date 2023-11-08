#include <imgui.h>
#include <ImGuizmo.h>
#include <spdlog/spdlog.h>
#include <Yonai/Window.hpp>
#include <Yonai/Graphics/Graphics.hpp>
#include <YonaiEditor/Systems/ImGUISystem.hpp>
#include <YonaiEditor/Systems/ImGUISystemBackend/ImGUIBackend_GLFW3.hpp>
#include <YonaiEditor/Systems/ImGUISystemBackend/ImGUIBackend_Vulkan.hpp>
#include <GLFW/glfw3.h>

using namespace glm;
using namespace Yonai;
using namespace Yonai::Systems;
using namespace Yonai::Graphics;
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

static void CheckVKResult(VkResult err)
{
	if (err == 0) return;
	spdlog::error("[ImGUI] Vulkan error: {}", (int)err);
}

static bool imguiInitialised = false;
void ImGUISystem::OnEnabled()
{
	if(!imguiInitialised)
	{
		switch (Yonai::Graphics::Graphics::API())
		{
		case GraphicsAPI::None: break;
		case GraphicsAPI::Vulkan:
			ImGui_ImplGlfw_InitForVulkan(Window::GetNativeHandle(), true);

			/*
			ImGui_ImplVulkan_InitInfo init_info = {};
			init_info.Instance = g_Instance;
			init_info.PhysicalDevice = g_PhysicalDevice;
			init_info.Device = g_Device;
			init_info.QueueFamily = g_QueueFamily;
			init_info.Queue = g_Queue;
			init_info.PipelineCache = g_PipelineCache;
			init_info.DescriptorPool = g_DescriptorPool;
			init_info.Subpass = 0;
			init_info.MinImageCount = g_MinImageCount;
			init_info.ImageCount = wd->ImageCount;
			init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
			init_info.Allocator = g_Allocator;

// #ifndef NDEBUG
			init_info.CheckVkResultFn = CheckVKResult;
// #endif

			ImGui_ImplVulkan_Init();
			*/
			break;
		}

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
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	imguiInitialised = false;
	m_IO = nullptr;
}

void ImGUISystem::Draw()
{
	EndFrame();
	StartFrame();
}

void ImGUISystem::StartFrame()
{
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGuizmo::BeginFrame();
}

void ImGUISystem::EndFrame()
{
	ImGui::EndFrame();
	ImGui::Render();

	ivec2 resolution = Window::GetFramebufferResolution();
	// glViewport(0, 0, resolution.x, resolution.y);
	// ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), );

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