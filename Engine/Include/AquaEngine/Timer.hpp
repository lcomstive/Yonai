#pragma once
#include <chrono>
#include <thread>
#include <functional>
#include <AquaEngine/API.hpp>

namespace AquaEngine
{
	typedef std::function<void()> TimerCallback;

	class Timer
	{
		bool m_IsRunning;
		std::thread m_Thread;
		TimerCallback m_Callback;
		std::chrono::milliseconds m_Interval;
		std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTime, m_StopTime;

	public:
		static const std::chrono::milliseconds DefaultInterval;

		Timer(unsigned int intervalMS);
		Timer(std::chrono::milliseconds interval = DefaultInterval);
		Timer(TimerCallback callback, std::chrono::milliseconds interval = DefaultInterval);
		~Timer();

		AquaAPI void Start();
		AquaAPI void Stop();
		AquaAPI std::chrono::milliseconds ElapsedTime();

		AquaAPI void Restart();

		AquaAPI bool IsRunning();
	};
}