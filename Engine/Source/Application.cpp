#include <filesystem>
#include <Yonai/Time.hpp>
#include <Yonai/Utils.hpp>
#include <Yonai/Window.hpp>
#include <Yonai/Application.hpp>
#include <Yonai/Scripting/Assembly.hpp>

// spdlog //
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>

// Systems //
#include <Yonai/SystemManager.hpp>
#include <Yonai/Systems/Global/SceneSystem.hpp>
#include <Yonai/Systems/Global/RenderSystem.hpp>

// Platform Specific //
#if defined(YONAI_PLATFORM_WINDOWS)
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
#elif defined(YONAI_PLATFORM_LINUX)
	#include <libgen.h>         // dirname
	#include <unistd.h>         // readlink
	#include <linux/limits.h>   // PATH_MAX
#elif defined(YONAI_PLATFORM_APPLE)
	#include <mach-o/dyld.h>
#endif

using namespace std;
using namespace Yonai;
using namespace Yonai::IO;
using namespace Yonai::Systems;

namespace fs = std::filesystem;

Application* Application::s_Instance = nullptr;

string Application::GetPersistentDirectory()
{
#if defined(YONAI_PLATFORM_WINDOWS)
#pragma warning(disable : 4996) // "This function may be unsafe"
	return string(getenv("appdata")) + "/Yonai/";
#pragma warning(default : 4996) // Restore warning
#elif defined(YONAI_PLATFORM_MAC)
		return string(getenv("HOME")) + "/Library/Caches/Yonai/";
#else
		return "./.data/Yonai/";
#endif
}

string LogFile = "Engine-Log.txt";
void Application::InitLogger()
{
	auto consoleSink = make_shared<spdlog::sinks::stdout_color_sink_mt>();
	consoleSink->set_pattern("%^[%=8n][%7l]%$ %v");

	auto logLevel = (spdlog::level::level_enum)stoi(GetArg("LogLevel",
#if !defined(NDEBUG) // Debug
		"0" // Trace
#else // Release
		"2" // Info
#endif
	));

	consoleSink->set_level(logLevel);

	auto fileSink = make_shared<spdlog::sinks::rotating_file_sink_mt>(GetPersistentDirectory() + LogFile, 1024 * 1024 /* 1MB max file size */, 3 /* Max files rotated */);
	fileSink->set_pattern("[%H:%M:%S %z][%t][%=8n][%7l] %v");
	fileSink->set_level(spdlog::level::trace);

	auto logger = make_shared<spdlog::logger>(spdlog::logger("Engine", { consoleSink, fileSink }));
	logger->set_level(spdlog::level::trace);
	spdlog::set_default_logger(logger);
}

Application::Application()
{
	InitLogger();

	s_Instance = this;
}

Application::Application(int argc, char** argv)
{
	ProcessArgs(argc, argv);

	InitLogger();

	s_Instance = this;
}

void Application::Setup()
{
	#pragma region Log engine information
	spdlog::info("{:>12} v{}.{}.{}-{} [{}]",
		"Yonai Engine",
		YONAI_VERSION_MAJOR,
		YONAI_VERSION_MINOR,
		YONAI_VERSION_PATCH,
		YONAI_VERSION_REV,
		YONAI_VERSION_BRANCH
	);
	spdlog::debug("{:>12}: {}", "Platform", YONAI_PLATFORM_NAME);
	spdlog::debug("");

	string persistentPath = GetPersistentDirectory();
	spdlog::debug("{:>12}: {}", "Persistent", persistentPath);
	spdlog::debug("{:>12}: {}", "Log File", persistentPath + LogFile);
	spdlog::debug("{:>12}: {}", "Launch Dir", std::filesystem::current_path().string());

	auto exePath = GetExecutablePath();
	spdlog::debug("{:>12}: {}", "Executable", exePath.string());

#if !defined(NDEBUG)
	spdlog::debug("{:>12}: {}", "Configuration", "Debug");
#else
	spdlog::debug("{:>12}: {}", "Configuration", "Release");
#endif

	spdlog::trace("Arguments:");
	for(auto& pair : m_Args)
	{
		if(pair.second.empty())
			spdlog::trace("  {}", pair.first);
		else
			spdlog::trace("  {}: {}", pair.first, pair.second);
	}
#pragma endregion
}

void Application::Cleanup() {}

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
	if(argc > 0)
	{
		m_ExecutablePath = fs::path(argv[0]);

		m_ExecutableDirectory = m_ExecutablePath.parent_path();

		#if defined(YONAI_PLATFORM_MAC)
		m_ExecutableDirectory = m_ExecutableDirectory.parent_path().append("Resources");
		#endif
	}

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
filesystem::path& Application::GetExecutableDirectory() { return m_ExecutableDirectory; }

#pragma region Windowed Application
void WindowedApplication::Setup()
{
	Application::Setup();
	SystemManager::Global()->Add<SceneSystem>();

	m_RenderSystem = SystemManager::Global()->Add<RenderSystem>();
	m_RenderSystem->GetPipeline()->SetResolution(Window::GetFramebufferResolution());

	Window::AddResizedCallback(OnWindowResized);
}

void WindowedApplication::Cleanup()
{
	Application::Cleanup();
	SystemManager::Global()->Remove<RenderSystem>();
	SystemManager::Global()->Remove<SceneSystem>();
}

RenderSystem* WindowedApplication::GetRenderSystem()
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
			
		OnPreDraw();
		SystemManager::Global()->Draw();
		OnPostDraw();

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

void WindowedApplication::OnWindowResized(glm::ivec2 resolution) { ((WindowedApplication*)Application::Current())->m_RenderSystem->GetPipeline()->SetResolution(resolution); }
#pragma endregion

#pragma region Managed Glue
#include <Yonai/Scripting/InternalCalls.hpp>
ADD_MANAGED_METHOD(Application, Exit)
{ Application::Current()->Exit(); }

ADD_MANAGED_METHOD(Application, GetArg, MonoString*, (MonoString* name, MonoString* defaultValue))
{
	string value = Application::Current()->GetArg(mono_string_to_utf8(name), mono_string_to_utf8(defaultValue));
	return mono_string_new(mono_domain_get(), value.c_str());
}

ADD_MANAGED_METHOD(Application, GetExecutablePath, MonoString*)
{ return mono_string_new(mono_domain_get(), Application::Current()->GetExecutablePath().string().c_str()); }

ADD_MANAGED_METHOD(Application, GetExecutableDirectory, MonoString*)
{ return mono_string_new(mono_domain_get(), Application::Current()->GetExecutableDirectory().string().c_str()); }

ADD_MANAGED_METHOD(Application, GetPersistentDirectory, MonoString*)
{ return mono_string_new(mono_domain_get(), Application::Current()->GetPersistentDirectory().c_str()); }

ADD_MANAGED_METHOD(Application, HasArg, bool, (MonoString* name))
{ return Application::Current()->HasArg(mono_string_to_utf8(name)); }

ADD_MANAGED_METHOD(Application, GetBuildType, unsigned char)
#ifndef NDEBUG // Debug mode
{ return 0; }
#else // Release mode
{ return 1; }
#endif

ADD_MANAGED_METHOD(Application, GetVersionMajor, int) { return YONAI_VERSION_MAJOR; }
ADD_MANAGED_METHOD(Application, GetVersionMinor, int) { return YONAI_VERSION_MINOR; }
ADD_MANAGED_METHOD(Application, GetVersionPatch, int) { return YONAI_VERSION_PATCH; }
ADD_MANAGED_METHOD(Application, GetVersionRevision, MonoString*) { return mono_string_new(mono_domain_get(), YONAI_VERSION_REV); }
ADD_MANAGED_METHOD(Application, GetVersionBranch, MonoString*) { return mono_string_new(mono_domain_get(), YONAI_VERSION_BRANCH); }

ADD_MANAGED_METHOD(Application, GetPlatform, unsigned char)
#if defined(YONAI_PLATFORM_WINDOWS)
{ return 1; }
#elif defined(YONAI_PLATFORM_MAC)
{ return 2; }
#elif defined(YONAI_PLATFORM_LINUX)
{ return 3; }
#elif defined(YONAI_PLATFORM_iOS)
{ return 4; }
#elif defined(YONAI_PLATFORM_ANDROID)
{ return 5; }
#elif defined(YONAI_PLATFORM_UNIX)
{ return 6; }
#else
{ return 0; }
#endif

ADD_MANAGED_METHOD(Application, IsDesktop, bool)
#if defined(YONAI_PLATFORM_DESKTOP)
{ return true; }
#else
{ return false; }
#endif

#pragma endregion
