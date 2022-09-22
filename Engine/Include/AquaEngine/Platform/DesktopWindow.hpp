/*
#pragma once
#include <AquaEngine/API.hpp>

#if defined(AQUA_PLATFORM_DESKTOP) && !defined(AQUA_ENGINE_NO_WINDOW)
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <unordered_map>
#include <AquaEngine/Graphics/Window.hpp>

namespace AquaEngine
{
	class Window
	{
		/// <summary>
		/// Window state
		/// </summary>
		WindowArgs m_State;

		/// <summary>
		/// Handle to window
		/// </summary>
		GLFWwindow* m_Handle;

#pragma region GLFW Callbacks
		static void GLFW_WindowCloseCallback(GLFWwindow* window);
		static void GLFW_ErrorCallback(int error, const char* description);
		static void GLFW_ScrollCallback(GLFWwindow* window, double xOffset, double yOffset);
		static void GLFW_MouseCallback(GLFWwindow* window, int button, int action, int mods);
		static void GLFW_CursorPositionCallback(GLFWwindow* window, double xPos, double yPos);
		static void GLFW_FramebufferResizeCallback(GLFWwindow* window, int width, int height);
		static void GLFW_KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void GLFW_DebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char* msg, const void* userParam);
#pragma endregion

		/// <summary>
		/// Maps GLFW-generated windows to Window class instances
		/// </summary>
		static std::unordered_map<GLFWwindow*, Window*> s_Instances;

	public:
		AquaAPI Window(WindowArgs& args);
		AquaAPI ~Window();

		AquaAPI void SwapBuffers();
		AquaAPI void PollEvents();

		AquaAPI bool RequestedToClose();

		AquaAPI void Show(bool show = true);
		AquaAPI void Close();

		AquaAPI void SetTitle(std::string title);
		AquaAPI void SetTitle(const char* title);
		AquaAPI std::string GetTitle();

		AquaAPI glm::ivec2 GetResolution();
		AquaAPI void SetResolution(glm::ivec2 resolution);
		AquaAPI void SetResolution(uint32_t width, uint32_t height);

		AquaAPI FullscreenMode GetFullscreen();
		AquaAPI void SetFullscreen(FullscreenMode mode);

		AquaAPI GLFWwindow* GetNativeHandle();
	};
}
#endif
*/