#pragma once
#include <filesystem>
#include <glm/glm.hpp>
#include <unordered_map>
#include <AquaEngine/API.hpp>
#include <AquaEngine/SystemManager.hpp>

namespace AquaEngine
{
	// Forward declaration
	namespace Systems { struct RenderSystem; }
		
	/// <summary>
	/// Base application class
	/// </summary>
	class Application
	{
		bool m_Running = true;
		std::filesystem::path m_ExecutablePath;
		std::filesystem::path m_ExecutableDirectory;
		std::unordered_map<std::string, std::string> m_Args = {};

		void InitLogger();

		static Application* s_Instance;

	protected:
		/// <summary>
		/// Called once per frame
		/// </summary>
		virtual void OnUpdate() { }

		/// <summary>
		/// Called before the main loop has begun
		/// </summary>
		AquaAPI virtual void Setup();
		
		/// <summary>
		/// Called after the main loop
		/// </summary>
		AquaAPI virtual void Cleanup();

		bool IsRunning();

	public:
		AquaAPI Application();
		AquaAPI Application(int argc, char** argv);
		AquaAPI virtual ~Application();

		/// <summary>
		/// Begins the engine loop, only returns when the application is closing
		/// </summary>
		AquaAPI virtual void Run();

		/// <summary>
		/// Stops the engine loop and releases resources
		/// </summary>
		AquaAPI void Exit();

		/// <summary>
		/// The path to the launched executable.
		/// </summary>
		AquaAPI std::filesystem::path& GetExecutablePath();

		/// <summary>
		/// The directory containing the launched executable.
		/// </summary>
		AquaAPI std::filesystem::path& GetExecutableDirectory();

		#pragma region Arguments
		AquaAPI void ProcessArgs(int argc, char** argv);

		AquaAPI bool HasArg(std::string name);

		/// <param name="name">Name of argument to search for, case-insensitive</param>
		/// <param name="default">Default value to return if argument is not found</param>
		AquaAPI std::string GetArg(std::string name, std::string defaultValue = "");
		#pragma endregion

		AquaAPI static Application* Current();

		AquaAPI static std::string GetPersistentDirectory();
	};

	/// <summary>
	/// Application that has a window
	/// </summary>
	class WindowedApplication : public Application
	{
		Systems::RenderSystem* m_RenderSystem = nullptr;

		static void OnWindowResized(glm::ivec2 resolution);

	public:
		WindowedApplication() : Application() { }
		WindowedApplication(int argc, char** argv) : Application(argc, argv) { }

	protected:
		AquaAPI virtual void OnPreDraw() { }
		AquaAPI virtual void OnPostDraw() { }
		AquaAPI Systems::RenderSystem* GetRenderSystem();

	public:
		AquaAPI virtual void Setup() override;
		AquaAPI virtual void Cleanup() override;

		AquaAPI virtual void Run() override;
	};
}