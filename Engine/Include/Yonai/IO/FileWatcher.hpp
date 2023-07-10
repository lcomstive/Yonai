// Based on https://solarianprogrammer.com/2019/01/13/cpp-17-filesystem-write-file-watcher-monitor/
#pragma once
#include <atomic>
#include <chrono>
#include <thread>
#include <string>
#include <functional>
#include <filesystem>
#include <unordered_map>
#include <Yonai/API.hpp>

namespace Yonai::IO
{
	enum class FileWatchStatus { Created, Removed, Modified };

	class FileWatcher
	{
		bool m_Multithread;
		std::string m_Path;
		std::thread m_LoopThread;
		std::atomic_bool m_Running;
		std::chrono::duration<int, std::milli> m_Interval; // Time between checking for changes
		std::function<void(const std::string&, FileWatchStatus)> m_Callback;
		std::unordered_map<std::string, std::filesystem::file_time_type> m_WatchedPaths;

		void BeginWatching();

	public:
		/// <param name="intervalMs">Time between checking for file changes, in milliseconds</param>
		YonaiAPI FileWatcher(std::string path, int intervalMs = 1000, bool multithread = true);

		// Constructor using default interval of 1000ms
		YonaiAPI FileWatcher(std::string path, bool multithread);

		YonaiAPI void Start(std::function<void(const std::string&, FileWatchStatus)> callback);
		YonaiAPI void Stop();
	};
}