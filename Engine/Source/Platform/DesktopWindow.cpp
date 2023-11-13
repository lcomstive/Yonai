/*
*
* Implementation of Window class for desktop platforms (Windows, Mac, Linux).
* 
* Uses the GLFW library to heavily simplify.
*
*/
#include <Yonai/API.hpp>

#if defined(YONAI_PLATFORM_DESKTOP) && !defined(YONAI_HEADLESS)
#include <Yonai/Input.hpp>
#include <Yonai/IO/Files.hpp>
#include <spdlog/spdlog.h>
#include <Yonai/Window.hpp>
#include <Yonai/Scripting/ScriptEngine.hpp>

using namespace std;
using namespace glm;
using namespace Yonai;
using namespace Yonai::Scripting;

string GamepadMappingPath = "GamepadMappings.txt";
Window* Window::s_Instance = nullptr;
bool Window::s_ContextInitialised = false;
vector<WindowResizeCallback> Window::s_WindowResizeCallbacks = {};
MonoMethod* Window::s_ManagedMethodScaled = nullptr;
MonoMethod* Window::s_ManagedMethodResized = nullptr;

void GLFWErrorCallback(int error, const char* message);

bool Window::InitContext()
{
	// Check if already initialised
	if (s_ContextInitialised)
		return true;

	if (glfwInit() == GLFW_FALSE)
	{
		spdlog::critical("Failed to initialise GLFW");
		return false;
	}

	ScriptEngine::AddReloadCallback(GetThunks);
	GetThunks();

	s_ContextInitialised = true;
	return true;
}

void Window::DestroyContext()
{
	if (s_ContextInitialised)
		glfwTerminate();
	s_ContextInitialised = false;
}

bool Window::ContextIsInitialised() { return s_ContextInitialised; }

Window::Window() :
	m_Handle(nullptr),
	m_Title("Yonai"),
	m_FullscreenMode(FullscreenMode::None)
{
	if (s_Instance)
	{
		spdlog::error("Tried creating a second window, this is not supported");
		return;
	}

	// Set instance to this
	s_Instance = this;

	// Set initial resolution
	m_Resolution.x = std::max(m_Resolution.x, 800);
	m_Resolution.x = std::max(m_Resolution.y, 600);

	if (!ContextIsInitialised())
		InitContext();

	// Window creation options //
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
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
	glfwSetWindowContentScaleCallback(m_Handle, GLFWWindowScaleCallback);
	glfwSetFramebufferSizeCallback(m_Handle, GLFWFramebufferResizeCallback);

	// Set VSync enabled by default
	SetVSync(true);

	// Update gamepad mappings from local file
	if (Yonai::IO::Exists(GamepadMappingPath))
	{
		if (glfwUpdateGamepadMappings(Yonai::IO::ReadText(GamepadMappingPath).c_str()))
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

	// Get content scaling of monitor/OS
	float xScaling, yScaling;
	glfwGetWindowContentScale(m_Handle, &xScaling, &yScaling);
	// Update values
	GLFWWindowScaleCallback(m_Handle, xScaling, yScaling);

	// Set initial video mode
	m_VideoMode = GetCurrentVideoMode();
}

Window* Window::Create() { return s_Instance ? s_Instance : (s_Instance = new Window()); }

void Window::Destroy()
{
	if (!s_Instance)
		return;
	s_Instance->Close();
	delete s_Instance;
	s_Instance = nullptr;
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

	// Clear pointers
	s_Instance->m_Handle = nullptr;
	s_Instance = nullptr;
}

void Window::GetThunks()
{
	s_ManagedMethodResized = mono_class_get_method_from_name(
		ScriptEngine::GetCoreAssembly()->GetClassFromName("Yonai", "Window"),
		"_OnResized", 0);
	s_ManagedMethodScaled = mono_class_get_method_from_name(
		ScriptEngine::GetCoreAssembly()->GetClassFromName("Yonai", "Window"),
		"_OnContentScaleChanged", 0);
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

float Window::GetRefreshRate() { return s_Instance ? s_Instance->m_VideoMode.RefreshRate : -1; }

ivec2 Window::GetFramebufferResolution(bool useImGui)
{
	ivec2 resolution(0, 0);
	if(!s_Instance)
		return resolution;
	
	glfwGetFramebufferSize(s_Instance->m_Handle, &resolution.x, &resolution.y);
	return resolution;
}

vec2 Window::GetContentScaling()
{
	if(!s_Instance)
		return {};

	vec2 output;
	glfwGetWindowContentScale(s_Instance->m_Handle, &output.x, &output.y);
	if(output != s_Instance->m_Scaling)
	{
		s_Instance->m_Scaling = output;
		mono_runtime_invoke(s_ManagedMethodScaled, nullptr, nullptr, nullptr);

		spdlog::trace("Window content scaling set to ({}, {})", output.x, output.y);
	}
	return s_Instance->m_Scaling;
}

void Window::CenterOnDisplay()
{
	if(!s_Instance || s_Instance->m_FullscreenMode == FullscreenMode::Fullscreen)
		return;

	// Get display resolution
	VideoMode videomode = GetCurrentVideoMode();

	// Calculate center, offset by half the window resolution
	ivec2 center = videomode.Resolution / 2 - s_Instance->m_Resolution / 2;

	// Set position
	glfwSetWindowPos(s_Instance->m_Handle, center.x, center.y);
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
		/* X Position	*/ 0,
		/* Y Position	*/ 0,
		/* Resolution X	*/ s_Instance->m_Resolution.x,
		/* Resolution Y */ s_Instance->m_Resolution.y,
		/* Refresh rate */ s_Instance->m_VSync ? videomode->refreshRate : GLFW_DONT_CARE
	);

	// If coming out of fullscreen, center window on monitor.
	// Done after glfw call so video mode is display's native resolution
	videomode = glfwGetVideoMode(monitor);
	if (!fullscreen)
		CenterOnDisplay();
}

bool Window::GetVSync() { return s_Instance ? s_Instance->m_VSync : false; }
void Window::SetVSync(bool enable)
{
	if (!s_Instance || s_Instance->m_VSync == enable)
		return;

	spdlog::trace("Setting VSync to {}", enable ? "on" : "off");

	s_Instance->m_VSync = enable;
	// glfwSwapInterval(s_Instance->m_VSync ? 1 : 0);
}

GLFWwindow* Window::GetNativeHandle() { return s_Instance ? s_Instance->m_Handle : nullptr; }

void Window::SwapBuffers()
{
	/*
	if (s_Instance && s_Instance->m_Handle)
		glfwSwapBuffers(s_Instance->m_Handle);
	*/
}

void Window::PollEvents()
{
	if (!s_Instance)
		return;
	
	Input::UpdateKeyStates();
	glfwPollEvents();
}

vector<VideoMode> Window::GetVideoModes()
{
	vector<VideoMode> output;

	if(!s_Instance)
		return output;
	
	GLFWmonitor* monitor = glfwGetWindowMonitor(s_Instance->m_Handle);
	if(!monitor)
		monitor = glfwGetPrimaryMonitor();

	int videoModeCount = 0;
	const GLFWvidmode* videomodes = glfwGetVideoModes(monitor, &videoModeCount);

	for (int i = 0; i < videoModeCount; i++)
		output.emplace_back(VideoMode
		{
			ivec2(videomodes[i].width, videomodes[i].height),
			(float)videomodes[i].refreshRate
		});

	return output;
}

VideoMode Window::GetCurrentVideoMode()
{
	if (!s_Instance)
		return {};

	GLFWmonitor* monitor = glfwGetWindowMonitor(s_Instance->m_Handle);
	if(!monitor)
		monitor = glfwGetPrimaryMonitor();

	const GLFWvidmode* videomode = glfwGetVideoMode(monitor);
	s_Instance->m_VideoMode =
	{
		ivec2(videomode->width, videomode->height),
		(float)videomode->refreshRate
	};

	return s_Instance->m_VideoMode;
}

void Window::SetVideoMode(const VideoMode mode)
{
	if(!s_Instance || s_Instance->m_FullscreenMode != FullscreenMode::Fullscreen)
		return;
	
	GLFWmonitor* monitor = glfwGetWindowMonitor(s_Instance->m_Handle);

	glfwSetWindowMonitor(
		s_Instance->m_Handle, monitor,
		0, 0,
		mode.Resolution.x, mode.Resolution.y,
		(int)mode.RefreshRate
	);

	s_Instance->m_VideoMode = mode;
	s_Instance->m_Resolution = mode.Resolution;
}

void Window::AddResizedCallback(WindowResizeCallback callback) { s_WindowResizeCallbacks.push_back(callback); }

#pragma region GLFW Callbacks
void Window::GLFWWindowCloseCallback(GLFWwindow* window) { s_Instance->Close(); }

void Window::GLFWErrorCallback(int error, const char* description)
{
	spdlog::error("[GLFW] {}", description);
}

#include <Yonai/Application.hpp>
void Window::GLFWFramebufferResizeCallback(GLFWwindow* glfwWindow, int width, int height)
{
	glm::ivec2 resolution = { width, height };
	s_Instance->m_Resolution = resolution;
	
	for (WindowResizeCallback callback : s_WindowResizeCallbacks)
		callback(resolution);

	mono_runtime_invoke(s_ManagedMethodResized, nullptr, nullptr, nullptr);
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
	case GLFW_PRESS:
		Input::s_KeyPressedThisFrame = true;
		Input::s_KeyStates[(Key)key] = Input::KeyState::Pressed;
		break;
	case GLFW_RELEASE:	Input::s_KeyStates[(Key)key] = Input::KeyState::Released; break;
	}
}

void Window::GLFWWindowScaleCallback(GLFWwindow* window, float xScale, float yScale)
{
	GLFWmonitor* monitor = glfwGetWindowMonitor(window);
	if(!monitor)
		monitor = glfwGetPrimaryMonitor();

	float monitorXScaling, monitorYScaling;
	glfwGetMonitorContentScale(monitor, &monitorXScaling, &monitorYScaling);

	xScale /= monitorXScaling;
	yScale /= monitorYScaling;

	if(s_Instance)
		s_Instance->m_Scaling = { xScale, yScale };

	mono_runtime_invoke(s_ManagedMethodScaled, nullptr, nullptr, nullptr);

	spdlog::trace("Window content scaling set to ({}, {})", xScale, yScale);
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
#pragma endregion

#endif