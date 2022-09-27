#include <filesystem>
#include <AquaEngine/Time.hpp>
#include <AquaEngine/Application.hpp>

// spdlog //
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>

// Systems //
#include <AquaEngine/SystemManager.hpp>
#include <AquaEngine/Systems/Global/SceneSystem.hpp>
#include <AquaEngine/Systems/Global/RenderSystem.hpp>
#include <AquaEngine/Systems/Global/ImGUISystem.hpp>

// Virtual File System //
#include <AquaEngine/IO/VFS.hpp>

#include <AquaEngine/Window.hpp>

using namespace std;
using namespace AquaEngine;
using namespace AquaEngine::IO;
using namespace AquaEngine::Systems;

string LogFile = "/PersistentData/Logs/Engine.txt";
void Application::InitLogger()
{
	auto consoleSink = make_shared<spdlog::sinks::stdout_color_sink_mt>();
	consoleSink->set_pattern("%^[%=8n][%7l]%$ %v");

#if !defined(NDEBUG) // Debug
	consoleSink->set_level(spdlog::level::trace);
#else // Release
	consoleSink->set_level(spdlog::level::info);
#endif

	auto fileSink = make_shared<spdlog::sinks::rotating_file_sink_mt>(VFS::GetAbsolutePath(LogFile), 1024 * 1024 /* 1MB max file size */, 3 /* Max files rotated */);
	fileSink->set_pattern("[%H:%M:%S %z][%t][%=8n][%7l] %v");
	fileSink->set_level(spdlog::level::trace);

	auto logger = make_shared<spdlog::logger>(spdlog::logger("Engine", { consoleSink, fileSink }));
	logger->set_level(spdlog::level::trace);
	spdlog::set_default_logger(logger);
}

void Application::InitVFS()
{
	VFSMapping* mapping = VFS::Mount("/PersistentData/",
#if defined(AQUA_PLATFORM_WINDOWS)
#pragma warning(disable : 4996) // "This function may be unsafe"
		string(getenv("appdata")) + "/Aqua Engine/"
#pragma warning(default : 4996) // Restore warning
#elif defined(AQUA_PLATFORM_MAC)
		string(getenv("HOME")) + "/Library/Caches/Aqua Engine/"
#else
		"./.data/"
#endif
	);

#if !defined(NDEBUG)
	// Debug mode, mount 'Apps/Assets/' to '/Assets/'
	#if defined(AQUA_PLATFORM_WINDOWS) 
		AquaEngine::IO::VFS::Mount("/Assets", "../../../Apps/Assets");
	#else
		AquaEngine::IO::VFS::Mount("/Assets", "../../Apps/Assets");
	#endif
#endif
}

Application::Application()
{
	InitVFS();
	InitLogger();

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
	spdlog::debug("{:>12}: {}", "Log File", VFS::GetAbsolutePath(LogFile));
	spdlog::debug("{:>12}: {}", "Persistent", VFS::GetMapping("/PersistentData/")->GetMountPath());
#ifdef AQUA_PLATFORM_DESKTOP
	spdlog::debug("{:>12}: {}", "Launch Dir", std::filesystem::current_path().string());
#endif

#if !defined(NDEBUG)
	spdlog::debug("{:>12}: {}", "Configuration", "Debug");
#endif

#if defined(AQUA_ENGINE_PLATFORM_MAC) || defined(AQUA_ENGINE_PLATFORM_LINUX)
	spdlog::debug("DYLD_LIBRARY_PATH = {}", getenv("DYLD_LIBRARY_PATH"));
#endif
#pragma endregion
}

void Application::Setup()
{
	SystemManager::Global()->Add<SceneSystem>();
}

void Application::Cleanup()
{
	SystemManager::Global()->Remove<SceneSystem>();
}

Application::~Application()
{
	SystemManager::Global()->Destroy();
	spdlog::shutdown();
}

bool Application::IsRunning() { return m_Running; }

void Application::Run()
{
	Setup();
	SystemManager::Global()->Init();

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

void ToLower(std::string& input) { transform(input.begin(), input.end(), input.begin(), ::tolower); }

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
			ToLower(name);
			m_Args.emplace(name, "");
		}
		else
		{
			string key = name.substr(0, splitPos);
			ToLower(key);
			m_Args.emplace(key, name.substr(splitPos + 1));
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
std::string& Application::GetArg(std::string name)
{
	ToLower(name);
	return m_Args[name];
}

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
		m_RenderSystem = SystemManager::Global()->Add<RenderSystem>();
	return m_RenderSystem;
}

void WindowedApplication::Run()
{
	Window::Create();

	// Check if window was created successfully
	if(!Window::GetNativeHandle())
		return;

	Setup();
	SystemManager::Global()->Init();

	while (IsRunning() && !Window::RequestedToClose())
	{
		// if OpenGL
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		OnUpdate();
		SystemManager::Global()->Update();
			
		SystemManager::Global()->Draw();
		OnDraw();

		Window::SwapBuffers();
		Window::PollEvents();

		Time::OnFrameEnd();
	}

	Window::Close();
	Cleanup();
	SystemManager::Global()->Destroy();
}
#pragma endregion