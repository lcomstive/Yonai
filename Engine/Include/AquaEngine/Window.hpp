/*
 *
 * Platform-specific window classes can be found in the `Platform` folder (e.g. DesktopWindow)
 *
 * These classes should be surrounded with #ifdef's to avoid conflict during compilation.
 *
 * Below is an empty Window class with the minimum required functionality
 *
 */
#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <AquaEngine/API.hpp>

#if defined(AQUA_PLATFORM_DESKTOP)
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define WINDOW_HANDLE_TYPE GLFWwindow*
#else
#define WINDOW_HANDLE_TYPE void*
#endif

namespace AquaEngine
{
	enum class FullscreenMode : int
	{
		None,
		Borderless,
		Fullscreen
	};

	struct VideoMode
	{
		glm::ivec2 Resolution;
		float RefreshRate;
	};

	struct Monitor
	{
		std::vector<VideoMode> VideoModes;
	};

	class Window
	{
		/// <summary>
		/// Native handle
		/// 
		/// Desktop: GLFWwindow*
		/// </summary>
		WINDOW_HANDLE_TYPE m_Handle;

		/// <summary>
		/// Window title
		/// </summary>
		std::string m_Title = "Aqua Application";

		/// <summary>
		/// Current video mode of the display
		/// </summary>
		VideoMode m_VideoMode = { { 1280, 720 }, 0 };

		/// <summary>
		/// Current resolution of the framebuffer
		/// </summary>
		glm::ivec2 m_Resolution = { 1280, 720 };
		
		/// <summary>
		/// Resolution when in windowed mode
		/// </summary>
		glm::ivec2 m_WindowedResolution = { 1280, 720 };

		/// <summary>
		/// Enable vertical sync
		/// </summary>
		bool m_VSync = false;

		/// <summary>
		/// Display mode of the window
		/// </summary>
		FullscreenMode m_FullscreenMode;

		/// <summary>
		/// Content scaling amount, retrieved from the system.
		/// This could be DPI, or Window's monitor scaling for instance
		/// </summary>
		glm::vec2 m_Scaling = { 1.0f, 1.0f };

		/// <summary>
		/// Global singleton instance
		/// </summary>
		static Window* s_Instance;

		/// <summary>
		/// Tracks if InitContext is called, and successful
		/// </summary>
		static bool s_ContextInitialised;

#if defined(AQUA_PLATFORM_DESKTOP)
		static void GLFWJoystickCallback(int jid, int event);
		static void GLFWWindowCloseCallback(GLFWwindow* window);
		static void GLFWMouseCallback(GLFWwindow*, int, int, int);
		static void GLFWScrollCallback(GLFWwindow*, double, double);
		static void GLFWKeyCallback(GLFWwindow*, int, int, int, int);
		static void GLFWErrorCallback(int error, const char* description);
		static void GLFWCursorPositionCallback(GLFWwindow*, double, double);
		static void GLFWWindowScaleCallback(GLFWwindow*, float xScale, float yScale);
		static void GLFWFramebufferResizeCallback(GLFWwindow* glfwWindow, int width, int height);
#endif

		Window();

	public:
		/// <summary>
		/// Sets up the context for window creation
		/// </summary>
		/// <returns>Success state</returns>
		AquaAPI static bool InitContext();

		/// <summary>
		/// Clears the context for window creation
		/// </summary>
		AquaAPI static void DestroyContext();

		/// <returns>True if <see cref="InitContext"/> has been called successfully</returns>
		AquaAPI static bool ContextIsInitialised();

		AquaAPI static Window* Create();
		AquaAPI static void Destroy();

		/// <summary>
		/// Has the user requested to close the window?
		/// </summary>
		AquaAPI static bool RequestedToClose();

		/// <summary>
		/// Close the window and release resources
		/// </summary>
		AquaAPI static void Close();

		/// <summary>
		/// Show or hide the window
		/// </summary>
		AquaAPI static void Show(bool show = true);

		/// <summary>
		/// Presents the backbuffer to the screen
		/// </summary>
		AquaAPI static void SwapBuffers();

		/// <summary>
		/// Checks for window events & updates
		/// </summary>
		AquaAPI static void PollEvents();

		AquaAPI static std::string GetTitle();
		AquaAPI static void SetTitle(std::string title);
		AquaAPI static void SetTitle(const char* title);

		/// <returns>Current refresh rate of the display, or -1 if no window instance</returns>
		AquaAPI static float GetRefreshRate();

		/// <returns>Size of the window, in pixels</returns>
		AquaAPI static glm::ivec2 GetResolution();

		AquaAPI static void SetResolution(glm::ivec2 resolution);
		AquaAPI static void SetResolution(uint32_t width, uint32_t height);
		
		/// <param name="useImGui">Whether to check ImGui window size, as opposed to the application window</param>
		/// <returns>Size of the window's framebuffer to draw to, in pixels</returns>
		AquaAPI static glm::ivec2 GetFramebufferResolution(bool useImGui = false);

		/// <returns>Current fullscreen mode</returns>
		AquaAPI static FullscreenMode GetFullscreen();

		/// <returns>Current video mode for the display the window is currently on</returns>
		AquaAPI static VideoMode GetCurrentVideoMode();

		/// <returns>All available video modes for the display the window is currently on</returns>
		AquaAPI static std::vector<VideoMode> GetVideoModes();

		/// <summary>
		/// Sets the display's video mode. Only works in exclusive fullscreen mode on desktop platforms.
		/// </summary>
		AquaAPI static void SetVideoMode(const VideoMode videoMode);

		/// <summary>
		/// Sets the fullscreen mode
		/// </summary>
		AquaAPI static void SetFullscreen(FullscreenMode mode);

		AquaAPI static bool GetVSync();
		AquaAPI static void SetVSync(bool enable);

		/// <summary>
		/// Content scaling amount, retrieved from the system.
		/// This could be DPI, or Window's monitor scaling for instance.
		/// 
		/// Returns (1.0, 1.0) on non-desktop platforms
		/// </summary>
		AquaAPI static glm::vec2 GetContentScaling();

		/// <summary>
		/// Places a window in the center of the display
		/// </summary>
		AquaAPI static void CenterOnDisplay();

		AquaAPI static WINDOW_HANDLE_TYPE GetNativeHandle();
	};
}