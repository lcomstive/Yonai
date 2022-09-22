#include <sstream>
#include <iomanip>
#include <spdlog/spdlog.h>
#include <AquaEngine/IO/VFS.hpp>
#include <AquaEngine/Profiler.hpp>

using namespace std;
using namespace chrono;
using namespace AquaEngine;
using namespace AquaEngine::IO;

mutex InlineProfiler::m_FileMutex;
string InlineProfiler::m_FilePath = "";
stringstream InlineProfiler::m_WriteStream;
unsigned int InlineProfiler::m_WriteCount = 0;
ofstream InlineProfiler::m_FileStream = {};

void InlineProfiler::Start(string filepath)
{
	if (!m_FilePath.empty())
	{
		spdlog::warn("Tried to start profiling session but did not stop previous one");
		return;
	}

	m_FilePath = VFS::GetAbsolutePath(filepath);

	m_FileStream = ofstream(m_FilePath, ios::trunc);
	if (!m_FileStream.is_open())
	{
		spdlog::warn("Failed to open profiler stream at '{}'", m_FilePath);
		m_FilePath = "";
		return;
	}

	WriteHeader();
}

void InlineProfiler::End()
{
	if (m_FilePath.empty())
		return; // Already ended, or didn't start

	WriteFooter();
	m_FilePath = "";
}

void InlineProfiler::WriteHeader()
{
	m_WriteCount = 0;
	m_WriteStream = stringstream();
	m_WriteStream << std::setprecision(3) << fixed;

	lock_guard guard(m_FileMutex);
	m_FileStream << "{ \"displayTimeUnits\": \"ns\", \"traceEvents\": [{}";
}

void InlineProfiler::WriteFooter()
{
	m_WriteStream << " ]}";

	lock_guard guard(m_FileMutex);
	m_FileStream << m_WriteStream.str();

	m_FileStream.flush();
	m_FileStream.close();
}

void InlineProfiler::EndScope(InlineProfileScope& scope)
{
	scope.SessionTimer.Stop();
	scope.End();

	bool tempSession = m_FilePath == "";

	if (tempSession)
	{
		spdlog::warn("No InlineProfiler session was started, overwriting default file");
		InlineProfiler::Start();
	}

	m_WriteStream << ", { ";
	m_WriteStream << "\"name\": \"" << scope.Name << "\", ";
	m_WriteStream << "\"cat\": \"PERF\", ";
	m_WriteStream << "\"ph\": \"X\", ";
	m_WriteStream << "\"ts\": " << duration_cast<microseconds>(scope.SessionTimer.ElapsedTime()).count() << ", ";
	m_WriteStream << "\"dur\": " << scope.SessionTimer.ElapsedTime().count() << ", ";
	m_WriteStream << "\"pid\": 0, ";
	m_WriteStream << "\"tid\": " << scope.Thread;
	m_WriteStream << " }";

	if (++m_WriteCount > WriteCountLimit)
	{
		lock_guard guard(m_FileMutex);
		m_FileStream << m_WriteStream.str();
		m_FileStream.flush();

		m_WriteCount = 0;
		m_WriteStream.str(string());
	}

	if (tempSession)
		InlineProfiler::End();
}

InlineProfiler::InlineProfileScope::InlineProfileScope(string name) : SessionTimer(), Name(name)
{
	Thread = this_thread::get_id();
}

InlineProfiler::InlineProfileScope::~InlineProfileScope() { End(); }

void InlineProfiler::InlineProfileScope::End()
{
	if (!SessionTimer.IsRunning())
		return;
	SessionTimer.Stop();
	InlineProfiler::EndScope(*this);

}
