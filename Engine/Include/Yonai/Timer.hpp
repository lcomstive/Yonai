#pragma once
#include <chrono>
#include <thread>
#include <functional>
#include <Yonai/API.hpp>

namespace Yonai
{
	typedef std::function<void()> TimerCallback;

	class Timer
	{
		bool m_IsRunning;
		std::thread m_Thread;
		TimerCallback m_Callback;
		std::chrono::milliseconds m_Interval;
		std::chrono::high_resolution_clock::time_point m_StartTime, m_StopTime;

	public:
		static const std::chrono::milliseconds DefaultInterval;

		Timer(unsigned int intervalMS);
		Timer(std::chrono::milliseconds interval = DefaultInterval);
		Timer(TimerCallback callback, std::chrono::milliseconds interval = DefaultInterval);
		~Timer();

		YonaiAPI void Start();
		YonaiAPI void Stop();
		YonaiAPI std::chrono::milliseconds ElapsedTime();

		YonaiAPI void Restart();

		YonaiAPI bool IsRunning();
	};
}