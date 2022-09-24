#include <filesystem>
#include <AquaEngine/Time.hpp>
#include <AquaEngine/Profiler.hpp>
#include <AquaEngine/Application.hpp>

// spdlog //
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>

// Systems //
#include <AquaEngine/SystemManager.hpp>
#include <AquaEngine/Systems/SceneSystem.hpp>
#include <AquaEngine/Systems/RenderSystem.hpp>
#include <AquaEngine/Systems/ImGUISystem.hpp>

// Virtual File System //
#include <AquaEngine/IO/VFS.hpp>

#include <AquaEngine/Window.hpp>

using namespace std;
using namespace AquaEngine;
using namespace AquaEngine::IO;
using namespace AquaEngine::Systems;

string LogFile = ".aqua/Logs/Engine.txt";
void Application::InitLogger()
{
	auto consoleSink = make_shared<spdlog::sinks::stdout_color_sink_mt>();
	consoleSink->set_pattern("%^[%=8n][%7l]%$ %v");

#if !defined(NDEBUG) // Debug
	consoleSink->set_level(spdlog::level::trace);
#else // Release
	consoleSink->set_level(spdlog::level::info);
#endif

	auto fileSink = make_shared<spdlog::sinks::rotating_file_sink_mt>(LogFile, 1024 * 1024 /* 1MB max file size */, 3 /* Max files rotated */);
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

#if !defined(NDEBUG) // Debug mode
	#if defined(AQUA_PLATFORM_WINDOWS) 
		AquaEngine::IO::VFS::Mount("/Assets", "../../../Apps/Assets");
	#else
		AquaEngine::IO::VFS::Mount("/Assets", "../../Apps/Assets");
	#endif
#endif

	// TODO: Map '/' to project root
	VFS::Mount("/Assets", "Assets");

	VFS::Mount("/Cache", ".aqua");
}

Application::Application()
{
	InitLogger();
	InitVFS();

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
	spdlog::info("{:>12}: {}", "Log File", LogFile);
#ifdef AQUA_PLATFORM_DESKTOP
	spdlog::info("{:>12}: {}", "Launch Dir", std::filesystem::current_path().string());
#endif

#if !defined(NDEBUG)
	spdlog::info("{:>12}: {}", "Configuration", "Debug");
#endif

#if defined(AQUA_ENGINE_PLATFORM_MAC) || defined(AQUA_ENGINE_PLATFORM_LINUX)
	spdlog::info("DYLD_LIBRARY_PATH = {}", getenv("DYLD_LIBRARY_PATH"));
#endif
#pragma endregion
}

void Application::Setup()
{
	SystemManager::Add<SceneSystem>();
}

void Application::Cleanup()
{
	SystemManager::Remove<SceneSystem>();
}

Application::~Application()
{
	spdlog::shutdown();
}

bool Application::IsRunning() { return m_Running; }

void Application::Run()
{
#ifndef NDEBUG
	InlineProfiler::Start("/PersistentData/engine_startup.json");
#endif

	Setup();
	SystemManager::Initialize();

#ifndef NDEBUG
	InlineProfiler::End();
	InlineProfiler::Start("/PersistentData/engine_profile.json");
#endif
	while (IsRunning())
	{
		OnUpdate();
		SystemManager::Update();

		Time::OnFrameEnd();
	}
#ifndef NDEBUG
	InlineProfiler::End();
	InlineProfiler::Start("/PersistentData/engine_shutdown.json");
#endif

	Cleanup();
	SystemManager::Destroy();

#ifndef NDEBUG
	InlineProfiler::End();
#endif
}

void Application::Exit() { m_Running = false; }

#pragma region Windowed Application
void WindowedApplication::Setup()
{
	Application::Setup();
	SystemManager::Add<ImGUISystem>();
	SystemManager::Add<RenderSystem>();
}

void WindowedApplication::Cleanup()
{
	Application::Cleanup();
	SystemManager::Remove<RenderSystem>();
	SystemManager::Remove<ImGUISystem>();
}

void WindowedApplication::Run()
{
#ifndef NDEBUG
	InlineProfiler::Start("/PersistentData/engine_startup.json");
#endif

	Window::Create();

	// Check if window was created successfully
	if(!Window::GetNativeHandle())
		return;

	Setup();
	SystemManager::Initialize();

#ifndef NDEBUG
	InlineProfiler::End();
	InlineProfiler::Start("/PersistentData/engine_profile.json");
#endif
	while (IsRunning() && !Window::RequestedToClose())
	{
		// if OpenGL
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#if defined(AQUA_PLATFORM_DESKTOP)
		glm::ivec2 resolution;
		glfwGetFramebufferSize(Window::GetNativeHandle(), &resolution.x, &resolution.y);
#else
		ivec2 resolution = Window::GetResolution();
#endif
		// glViewport(0, 0, resolution.x, resolution.y);

		OnUpdate();
		SystemManager::Update();
		SystemManager::Draw();
		OnDraw();

		Window::SwapBuffers();
		Window::PollEvents();

		Time::OnFrameEnd();
	}
#ifndef NDEBUG
	InlineProfiler::End();
	InlineProfiler::Start("/PersistentData/engine_shutdown.json");
#endif

	Window::Close();
	Cleanup();
	SystemManager::Destroy();

#ifndef NDEBUG
	InlineProfiler::End();
#endif
}
#pragma endregion