#pragma once
#include <chrono>
#include <AquaEngine/API.hpp>

namespace AquaEngine
{
	class Time
	{
		/// <summary>
		/// Frames counted since the last second
		/// </summary>
		static int s_FrameCount;

		/// <summary>
		/// Time since the last second passed
		/// </summary>
		static double s_FrameTimeAccumulator;

		/// <summary>
		/// Time at which the current frame started
		/// </summary>
		static std::chrono::time_point<std::chrono::high_resolution_clock> s_FrameStartTime;

		/// <summary>
		/// Frames per second
		/// </summary>
		static double s_FPS;

		/// <summary>
		/// Time since the last frame
		/// </summary>
		static double s_DeltaTime;

		/// <summary>
		/// Time since the application started
		/// </summary>
		static double s_TimePassed;

	public:
		/// <summary>
		/// Frames per second
		/// </summary>
		AquaAPI static double FPS();

		/// <summary>
		/// Time taken to process the previous frame, in seconds
		/// </summary>
		AquaAPI static double DeltaTime();

		/// <summary>
		/// Time since the application launched, in seconds
		/// </summary>
		AquaAPI static double SinceLaunch();

		/// <summary>
		/// Call once per frame!
		/// 
		/// Used to calculate FPS & delta time.
		/// Expected to only be called inside the engine or application loop
		/// (e.g. Application::Run)
		/// </summary>
		AquaAPI static void OnFrameEnd();
	};
}