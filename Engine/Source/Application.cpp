#include <filesystem>
#include <AquaEngine/Time.hpp>
#include <AquaEngine/Utils.hpp>
#include <AquaEngine/Application.hpp>
#include <AquaEngine/Scripting/Assembly.hpp>

// spdlog //
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>

// Systems //
#include <AquaEngine/SystemManager.hpp>
#include <AquaEngine/Systems/Global/ImGUISystem.hpp>
#include <AquaEngine/Systems/Global/SceneSystem.hpp>
#include <AquaEngine/Systems/Global/RenderSystem.hpp>

// Virtual File System //
#include <AquaEngine/IO/VFS.hpp>

#include <AquaEngine/Window.hpp>

// Platform Specific //
#if defined(AQUA_PLATFORM_WINDOWS)
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
#elif defined(AQUA_PLATFORM_LINUX)
	#include <libgen.h>         // dirname
	#include <unistd.h>         // readlink
	#include <linux/limits.h>   // PATH_MAX
#elif defined(AQUA_PLATFORM_APPLE)
	#include <mach-o/dyld.h>
#endif

using namespace std;
using namespace AquaEngine;
using namespace AquaEngine::IO;
using namespace AquaEngine::Systems;

namespace fs = std::filesystem;

Application* Application::s_Instance = nullptr;

string GetPersistentDir()
{
#if defined(AQUA_PLATFORM_WINDOWS)
#pragma warning(disable : 4996) // "This function may be unsafe"
		return string(getenv("appdata")) + "/Aqua Engine/";
#pragma warning(default : 4996) // Restore warning
#elif defined(AQUA_PLATFORM_MAC)
		return string(getenv("HOME")) + "/Library/Caches/Aqua Engine/";
#else
		return "./.data/";
#endif
}

string LogFile = "Engine-Log.txt";
void Application::InitLogger()
{
	auto consoleSink = make_shared<spdlog::sinks::stdout_color_sink_mt>();
	consoleSink->set_pattern("%^[%=8n][%7l]%$ %v");

#if !defined(NDEBUG) // Debug
	consoleSink->set_level(spdlog::level::trace);
#else // Release
	consoleSink->set_level(spdlog::level::info);
#endif

	auto fileSink = make_shared<spdlog::sinks::rotating_file_sink_mt>(GetPersistentDir() + LogFile, 1024 * 1024 /* 1MB max file size */, 3 /* Max files rotated */);
	fileSink->set_pattern("[%H:%M:%S %z][%t][%=8n][%7l] %v");
	fileSink->set_level(spdlog::level::trace);

	auto logger = make_shared<spdlog::logger>(spdlog::logger("Engine", { consoleSink, fileSink }));
	logger->set_level(spdlog::level::trace);
	spdlog::set_default_logger(logger);
}

void Application::InitVFS()
{
	// Map persistent data
	VFSMapping* mapping = VFS::Mount("data://", GetPersistentDir());

	// Get executable path and directory
#if defined(AQUA_PLATFORM_WINDOWS)
	wchar_t exeResult[MAX_PATH];
	if (GetModuleFileNameW(NULL, exeResult, MAX_PATH) > 0)
#elif defined(AQUA_PLATFORM_LINUX)
	char exeResult[PATH_MAX];
	if (readlink("/proc/self/exe", exeResult, PATH_MAX) > 0)
#elif defined(AQUA_PLATFORM_APPLE)
	char exeResult[PATH_MAX];
	uint32_t exeResultSize = PATH_MAX;
	if (_NSGetExecutablePath(exeResult, &exeResultSize) == 0)
#endif
		m_ExecutablePath = std::filesystem::path(exeResult);

	// Map app:// to launched executable directory
#if defined(AQUA_PLATFORM_APPLE)
	fs::path executableDir = m_ExecutablePath.parent_path();
	if(executableDir.filename().compare("MacOS") == 0) // App Bundle
		// Assets for app are found in Resources subfolder in the app bundle
		VFS::Mount("app://", (executableDir.parent_path() / "Resources").string());
	else
		VFS::Mount("app://", executableDir.string());
#else
	VFS::Mount("app://", m_ExecutablePath.parent_path().string());
#endif

	VFS::Mount("file:///", "");
}

Application::Application()
{
	InitLogger();
	InitVFS();

	s_Instance = this;
}

void Application::Setup()
{
	SystemManager::Global()->Add<SceneSystem>();

	#pragma region Log engine information
	spdlog::info("{:>12}: v{}.{}.{}-{} [{}]",
		"Engine",
		AQUA_ENGINE_VERSION_MAJOR,
		AQUA_ENGINE_VERSION_MINOR,
		AQUA_ENGINE_VERSION_PATCH,
		AQUA_ENGINE_VERSION_REV,
		AQUA_ENGINE_VERSION_BRANCH
	);
	spdlog::info("{:>12}: {}", "Platform", AQUA_PLATFORM_NAME);
	spdlog::info("");

	string persistentPath = VFS::GetMapping("data://")->GetMountPath() + "/";
	spdlog::debug("{:>12}: {}", "Persistent", persistentPath);
	spdlog::debug("{:>12}: {}", "Log File", persistentPath + LogFile);
	spdlog::debug("{:>12}: {}", "Launch Dir", std::filesystem::current_path().string());

	auto exePath = GetExecutablePath();
	spdlog::debug("{:>12}: {}", "Executable", exePath.string());

#if !defined(NDEBUG)
	spdlog::debug("{:>12}: {}", "Configuration", "Debug");
#endif
#pragma endregion
}

void Application::Cleanup()
{
	SystemManager::Global()->Remove<SceneSystem>();
}

Application::~Application()
{
	SystemManager::Global()->Destroy();
	spdlog::shutdown();

	s_Instance = nullptr;
}

bool Application::IsRunning() { return m_Running; }

void Application::Run()
{
	Setup();
	SystemManager::Global()->Enable(true);

	while (IsRunning())
	{
		OnUpdate();

		SystemManager::Global()->Update();

		Time::OnFrameEnd();
	}

	Cleanup();
	SystemManager::Global()->Destroy();
}

void Application::Exit() { m_Running = false; }
Application* Application::Current() { return s_Instance; }

void Application::ProcessArgs(int argc, char** argv)
{
	size_t previousArgCount = m_Args.size();
	for(int i = 0; i < argc; i++)
	{
		string name(argv[i]);

		// Accept parameters starting with '-' && '--'
		if(name[0] != '-')
			continue;

		for(int i = 0; i < 2; i++)
			if(name[0] == '-')
				name.erase(0, 1);

		auto splitPos = name.find('=');
		if(splitPos == string::npos)
		{
			// Add key without value as argument,
			// this typically represents a flag, like '-headless'
			ToLower(name);
			m_Args.emplace(name, "");
		}
		else
		{
			// Extract key
			string key = name.substr(0, splitPos);
			ToLower(key);

			// Extract value
			string value = name.substr(splitPos + 1);
			// Remove quotations, if present
			if(value[0] == '"' && value[value.size() - 1] == '"')
				value = value.substr(1, value.size() - 2);

			// Add to list of found arguments
			m_Args.emplace(key, value);
		}
	}

	if(m_Args.size() == previousArgCount)
		return; // Nothing new added

	spdlog::debug("Added {} argument(s)", m_Args.size());
	for(auto& pair : m_Args)
	{
		if(pair.second.empty())
			spdlog::debug("  '{}'", pair.first);
		else
			spdlog::debug("  '{}' = '{}'", pair.first, pair.second);
	}
}

bool Application::HasArg(std::string name)
{
	ToLower(name);
	return m_Args.find(name) != m_Args.end();
}

std::string Application::GetArg(string name, string defaultValue)
{
	ToLower(name);
	return HasArg(name) ? m_Args[name] : defaultValue;
}

filesystem::path& Application::GetExecutablePath() { return m_ExecutablePath; }

#pragma region Windowed Application
void WindowedApplication::Setup()
{
	Application::Setup();
	SystemManager::Global()->Add<ImGUISystem>();

	m_RenderSystem = SystemManager::Global()->Add<RenderSystem>();
	m_RenderSystem->GetPipeline()->SetResolution(Window::GetFramebufferResolution());
}

void WindowedApplication::Cleanup()
{
	Application::Cleanup();
	SystemManager::Global()->Remove<RenderSystem>();
	SystemManager::Global()->Remove<ImGUISystem>();
}

AquaAPI RenderSystem* WindowedApplication::GetRenderSystem()
{
	if (!m_RenderSystem)
	{
		m_RenderSystem = SystemManager::Global()->Add<RenderSystem>();
		spdlog::debug("No render system found in application, creating one");
	}
	return m_RenderSystem;
}

void WindowedApplication::Run()
{
	Window::Create();

	// Check if window was created successfully
	if(!Window::GetNativeHandle())
		return;

	Setup();
	SystemManager::Global()->Enable(true);

	while (IsRunning() && !Window::RequestedToClose())
	{
		// if OpenGL
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		OnUpdate();
		SystemManager::Global()->Update();
			
		OnDraw();
		SystemManager::Global()->Draw();

		Window::SwapBuffers();
		Window::PollEvents();

		Time::OnFrameEnd();
	}

	Window::Close();
	Cleanup();
	SystemManager::Global()->Destroy();

	if(Window::ContextIsInitialised())
		Window::DestroyContext();
}
#pragma endregion

#pragma region Managed Glue
#include <AquaEngine/Scripting/InternalCalls.hpp>
ADD_MANAGED_METHOD(Application, Exit)
{ Application::Current()->Exit(); }

ADD_MANAGED_METHOD(Application, GetArg, MonoString*, (MonoString* name, MonoString* defaultValue))
{
	string value = Application::Current()->GetArg(mono_string_to_utf8(name), mono_string_to_utf8(defaultValue));
	return mono_string_new(mono_domain_get(), value.c_str());
}

ADD_MANAGED_METHOD(Application, HasArg, bool, (MonoString* name))
{ return Application::Current()->HasArg(mono_string_to_utf8(name)); }
#pragma endregion