#pragma once
#include <filesystem>
#include <glm/glm.hpp>
#include <unordered_map>
#include <Yonai/API.hpp>
#include <Yonai/SystemManager.hpp>

namespace Yonai
{		
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
		YonaiAPI virtual void Setup();
		
		/// <summary>
		/// Called after the main loop
		/// </summary>
		YonaiAPI virtual void Cleanup();

		bool IsRunning();

	public:
		YonaiAPI Application();
		YonaiAPI Application(int argc, char** argv);
		YonaiAPI virtual ~Application();

		/// <summary>
		/// Begins the engine loop, only returns when the application is closing
		/// </summary>
		YonaiAPI virtual void Run();

		/// <summary>
		/// Stops the engine loop and releases resources
		/// </summary>
		YonaiAPI void Exit();

		/// <summary>
		/// The path to the launched executable.
		/// </summary>
		YonaiAPI std::filesystem::path& GetExecutablePath();

		/// <summary>
		/// The directory containing the launched executable.
		/// </summary>
		YonaiAPI std::filesystem::path& GetExecutableDirectory();

		#pragma region Arguments
		YonaiAPI void ProcessArgs(int argc, char** argv);

		YonaiAPI bool HasArg(std::string name);

		/// <param name="name">Name of argument to search for, case-insensitive</param>
		/// <param name="default">Default value to return if argument is not found</param>
		YonaiAPI std::string GetArg(std::string name, std::string defaultValue = "");
		#pragma endregion

		YonaiAPI static Application* Current();

		YonaiAPI static std::string GetPersistentDirectory();
	};

	/// <summary>
	/// Application that has a window
	/// </summary>
	class WindowedApplication : public Application
	{
		static void OnWindowResized(glm::ivec2 resolution);

	public:
		WindowedApplication() : Application() { }
		WindowedApplication(int argc, char** argv) : Application(argc, argv) { }

	protected:
		YonaiAPI virtual void OnPreDraw() { }
		YonaiAPI virtual void OnPostDraw() { }

	public:
		YonaiAPI virtual void Setup() override;
		YonaiAPI virtual void Cleanup() override;

		YonaiAPI virtual void Run() override;
	};
}