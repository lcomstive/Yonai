/*
*
* Implementation of Window class for desktop platforms (Windows, Mac, Linux).
* 
* Uses the GLFW library to heavily simplify.
*
*/

#include <AquaEngine/Input.hpp>
#include <AquaEngine/IO/Files.hpp>

#if defined(AQUA_PLATFORM_DESKTOP) && !defined(AQUA_ENGINE_HEADLESS)
#include <imgui.h>
#include <spdlog/spdlog.h>
#include <imgui_internal.h>
#include <AquaEngine/Window.hpp>

using namespace std;
using namespace glm;
using namespace AquaEngine;

string GamepadMappingPath = "GamepadMappings.txt";
Window* Window::s_Instance = nullptr;

void GLFWErrorCallback(int error, const char* message);
void GLFWDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char* msg, const void* userParam);

Window::Window() :
	m_Handle(nullptr),
	m_Title("Aqua Engine"),
	m_Resolution({ 800, 600 }),
	m_FullscreenMode(FullscreenMode::None)
{
	if (s_Instance)
		spdlog::error("A window is already created. This is currently untested and may not work as intended.");

	if (glfwInit() == GLFW_FALSE)
	{
		spdlog::critical("Failed to initialise GLFW");
		return;
	}

	// Ensure input resolution is at least 800x600
	m_Resolution.x = std::max(m_Resolution.x, 800);
	m_Resolution.x = std::max(m_Resolution.y, 600);

	// Window creation options //
	// Set as resizable
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	// Set preferred OpenGL version 
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
#if !defined(AQUA_PLATFORM_APPLE)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
#else
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
	glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif

#if !defined(NDEBUG)
	// When in debug configuration, enable OpenGL's debugging
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif

	glfwSetErrorCallback(GLFWErrorCallback);

	m_Handle = glfwCreateWindow(
		m_Resolution.x,
		m_Resolution.y,
		m_Title.c_str(),
		nullptr, nullptr // Monitor, share (?)
	);

	if(!m_Handle)
	{
		spdlog::critical("Failed to create window!");
		return;
	}

	// Register GLFW callbacks
	glfwSetJoystickCallback(GLFWJoystickCallback);
	glfwSetKeyCallback(m_Handle,			 GLFWKeyCallback);
	glfwSetMouseButtonCallback(m_Handle,	 GLFWMouseCallback);
	glfwSetScrollCallback(m_Handle,			 GLFWScrollCallback);
	glfwSetWindowCloseCallback(m_Handle,	 GLFWWindowCloseCallback);
	glfwSetCursorPosCallback(m_Handle,		 GLFWCursorPositionCallback);
	glfwSetFramebufferSizeCallback(m_Handle, GLFWFramebufferResizeCallback);

	// Finalise OpenGL creation
	glfwMakeContextCurrent(m_Handle);

	int glVersion = gladLoadGL(glfwGetProcAddress);
	if (glVersion == 0)
	{
		spdlog::critical("Failed to initialise OpenGL context");
		return;
	}
#if !defined(NDEBUG) && !defined(__APPLE__)
	// If debug configuration, enable OpenGL debug output
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(GLFWDebugOutput, nullptr);
#endif

	// Output device information in debug mode
	spdlog::debug("OpenGL v{}.{}", GLAD_VERSION_MAJOR(glVersion), GLAD_VERSION_MINOR(glVersion));
	spdlog::debug("\tDevice: {} (driver {})", (const char*)glGetString(GL_RENDERER), (const char*)glGetString(GL_VERSION));

	// Set instance to this
	s_Instance = this;

	// Update gamepad mappings from local file
	if (AquaEngine::IO::Exists(GamepadMappingPath))
	{
		if (glfwUpdateGamepadMappings(AquaEngine::IO::ReadText(GamepadMappingPath).c_str()))
			spdlog::debug("Updated gamepad mappings from file '{}'", GamepadMappingPath);
		else
			spdlog::warn("Failed to update gamepad mapping from file '{}'", GamepadMappingPath);
	}

	// Iterate over all joysticks and check if connected
	for (int i = 0; i < GLFW_JOYSTICK_LAST; i++)
	{
		if (glfwJoystickPresent(i))
			GLFWJoystickCallback(i, GLFW_CONNECTED);
		else
			Input::s_ConnectedGamepads[i] = Input::JoystickType::Disconnected;
	}
}

Window* Window::Create() { return s_Instance ? s_Instance : (s_Instance = new Window()); }

void Window::Destroy()
{
	if (!s_Instance)
		return;
	s_Instance->Close();
	delete s_Instance;
}

bool Window::RequestedToClose() { return s_Instance && s_Instance->m_Handle ? glfwWindowShouldClose(s_Instance->m_Handle) : true; }

void Window::Show(bool show)
{
	if (!s_Instance || !s_Instance->m_Handle)
		return;

	if (show)
		glfwShowWindow(s_Instance->m_Handle);
	else
		glfwHideWindow(s_Instance->m_Handle);
}

void Window::Close()
{
	if (!s_Instance || !s_Instance->m_Handle)
		return;

	// Release GLFW resources
	glfwDestroyWindow(s_Instance->m_Handle);
	glfwTerminate();

	// Clear pointers
	s_Instance->m_Handle = nullptr;
	s_Instance = nullptr;
}

void Window::SetTitle(std::string title)
{
	if(s_Instance)
		s_Instance->m_Title = title;

	if (s_Instance && s_Instance->m_Handle)
		glfwSetWindowTitle(s_Instance->m_Handle, title.c_str());
}

void Window::SetTitle(const char* title) { SetTitle(string(title)); }

string Window::GetTitle() { return s_Instance ? s_Instance->m_Title : ""; }

ivec2 Window::GetResolution() { return s_Instance ? s_Instance->m_Resolution : glm::ivec2(0, 0); }

ivec2 Window::GetFramebufferResolution(bool useImGui)
{
	ivec2 resolution(0, 0);
	if(!s_Instance)
		return resolution;

	if(useImGui && GImGui && GImGui->CurrentWindow)
	{
		// If inside ImGui window, get window size
		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		return { viewportPanelSize.x, viewportPanelSize.y };
	}
	
	glfwGetFramebufferSize(s_Instance->m_Handle, &resolution.x, &resolution.y);
	return resolution;
}

void Window::SetResolution(glm::ivec2 resolution)
{
	if (!s_Instance)
		return;

	s_Instance->m_Resolution = resolution;
	if (s_Instance->m_FullscreenMode == FullscreenMode::None)
		s_Instance->m_WindowedResolution = resolution;

	if(s_Instance->m_Handle)
		glfwSetWindowSize(s_Instance->m_Handle, resolution.x, resolution.y);
}

void Window::SetResolution(uint32_t width, uint32_t height) { SetResolution({ width, height }); }

FullscreenMode Window::GetFullscreen() { return s_Instance ? s_Instance->m_FullscreenMode : FullscreenMode::None; }
void Window::SetFullscreen(FullscreenMode mode)
{
	if (!s_Instance || mode == s_Instance->m_FullscreenMode)
		return; // No change

	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* videomode = glfwGetVideoMode(monitor);

	bool fullscreen = mode != FullscreenMode::None;

	if (s_Instance->m_FullscreenMode == FullscreenMode::None)
		s_Instance->m_WindowedResolution = s_Instance->m_Resolution;
	else if (mode == FullscreenMode::None)
		s_Instance->m_Resolution = s_Instance->m_WindowedResolution;
	
	if (mode == FullscreenMode::Borderless)
		s_Instance->m_Resolution = { videomode->width, videomode->height };

	s_Instance->m_FullscreenMode = mode;

	spdlog::trace("Setting fullscreen mode to {} ({}x{})", (int)mode, s_Instance->m_Resolution.x, s_Instance->m_Resolution.y);

	glfwSetWindowMonitor(
		/* GLFW window	*/ s_Instance->m_Handle,
		/* Monitor		*/ fullscreen ? monitor : nullptr,
		/* X Position	*/ fullscreen ? 0 : int(videomode->width  / 2.0f - s_Instance->m_Resolution.x / 2.0f),
		/* Y Position	*/ fullscreen ? 0 : int(videomode->height / 2.0f - s_Instance->m_Resolution.y / 2.0f),
		/* Resolution X	*/ s_Instance->m_Resolution.x,
		/* Resolution Y */ s_Instance->m_Resolution.y,
		/* Refresh rate */ s_Instance->m_VSync ? videomode->refreshRate : GLFW_DONT_CARE
	);

	glViewport(0, 0, s_Instance->m_Resolution.x, s_Instance->m_Resolution.y);
}

bool Window::GetVSync() { return s_Instance ? s_Instance->m_VSync : false; }
void Window::SetVSync(bool enable)
{
	if (!s_Instance)
		return;

	spdlog::trace("Setting VSync to {}", enable ? "on" : "off");

	s_Instance->m_VSync = enable;
	glfwSwapInterval(s_Instance->m_VSync ? 1 : 0);
}

GLFWwindow* Window::GetNativeHandle() { return s_Instance ? s_Instance->m_Handle : nullptr; }

void Window::SwapBuffers()
{
	if (s_Instance && s_Instance->m_Handle)
		glfwSwapBuffers(s_Instance->m_Handle);
}

void Window::PollEvents()
{
	if (!s_Instance)
		return;
	
	Input::UpdateKeyStates();
	glfwPollEvents();
}

#pragma region GLFW Callbacks
void Window::GLFWWindowCloseCallback(GLFWwindow* window) { s_Instance->Close(); }

void Window::GLFWErrorCallback(int error, const char* description)
{
	spdlog::error("[GLFW] {}", description);
}

void Window::GLFWFramebufferResizeCallback(GLFWwindow* glfwWindow, int width, int height)
{
	s_Instance->m_Resolution = { width, height };
	
	glViewport(0, 0, width, height);
}

void Window::GLFWScrollCallback(GLFWwindow* window, double xOffset, double yOffset) { Input::s_ScrollDelta += (float)yOffset; }

void Window::GLFWMouseCallback(GLFWwindow* window, int button, int action, int mods)
{
	switch (action)
	{
	default: break;
	case GLFW_PRESS:	Input::s_MouseStates[button] = Input::KeyState::Pressed; break;
	case GLFW_RELEASE:	Input::s_MouseStates[button] = Input::KeyState::Released; break;
	}
}

void Window::GLFWCursorPositionCallback(GLFWwindow* window, double xPos, double yPos) { Input::s_MousePosition = { xPos, yPos }; }

void Window::GLFWKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	switch (action)
	{
	default: break;
	case GLFW_PRESS:	Input::s_KeyStates[(Key)key] = Input::KeyState::Pressed; break;
	case GLFW_RELEASE:	Input::s_KeyStates[(Key)key] = Input::KeyState::Released; break;
	}
}

void Window::GLFWJoystickCallback(int jid, int event)
{
	Input::s_ConnectedGamepads[jid] =
		event == GLFW_CONNECTED ?
		(glfwJoystickIsGamepad(jid) ? Input::JoystickType::Gamepad : Input::JoystickType::Joystick)
		: Input::JoystickType::Disconnected;

	if (event == GLFW_CONNECTED)
	{
		spdlog::debug("[#{}] '{}' was connected",
			jid,
			glfwGetJoystickName(jid)
		);

		GLFWgamepadstate state;
		if (!glfwGetGamepadState(jid, &state))
			spdlog::warn("No mapping found for device [#{}] '{}'", jid, glfwGetJoystickName(jid));
	}
	else
		spdlog::debug("Gamepad #{} was disconnected", jid);
}

void GLFWErrorCallback(int error, const char* message)
{
	spdlog::error("[GLFW] {} [{}]", message, error);
}

void GLFWDebugOutput(
	GLenum source,
	GLenum type,
	unsigned int id,
	GLenum severity,
	GLsizei length,
	const char* msg,
	const void* userParam)
{
	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:
		spdlog::error(msg);
		break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
	case GL_DEBUG_TYPE_PERFORMANCE:
		spdlog::warn(msg);
		break;
	case GL_DEBUG_TYPE_OTHER:
		break;
	default:
		spdlog::info(msg);
		break;
	}
}
#pragma endregion

#endif