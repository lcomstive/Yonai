#pragma once
#include <AquaEngine/API.hpp>

namespace AquaEngine
{
	/// <summary>
	/// Base application class
	/// </summary>
	class Application
	{
		bool m_Running = true;

		void InitVFS();
		void InitLogger();

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
		AquaAPI virtual ~Application();

		/// <summary>
		/// Begins the engine loop, only returns when the application is closing
		/// </summary>
		AquaAPI virtual void Run();

		/// <summary>
		/// Stops the engine loop and releases resources
		/// </summary>
		AquaAPI void Exit();
	};

	/// <summary>
	/// Application that has a window
	/// </summary>
	class WindowedApplication : public Application
	{
	protected:
		AquaAPI virtual void OnDraw() { }

	public:
		AquaAPI virtual void Setup() override;
		AquaAPI virtual void Cleanup() override;

		AquaAPI virtual void Run() override;
	};
}