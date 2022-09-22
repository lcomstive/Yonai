#pragma once
#include <map>
#include <string>
#include <thread>
#include <fstream>
#include <spdlog/spdlog.h>
#include <AquaEngine/API.hpp>
#include <AquaEngine/Timer.hpp>

using namespace std;

namespace AquaEngine
{
	class InlineProfiler
	{
	private:
		static string m_FilePath;
		static mutex m_FileMutex;
		static ofstream m_FileStream;
		static unsigned int m_WriteCount;
		static stringstream m_WriteStream;

		static const unsigned int WriteCountLimit = 500;

		static void WriteHeader();
		static void WriteFooter();

	public:
		struct InlineProfileScope
		{
			Timer SessionTimer;
			string Name;
			thread::id Thread;

			InlineProfileScope(string name);
			~InlineProfileScope();

			void End();
		};

		static void Start(string filepath = "profile.json");
		static void End();

		static void EndScope(InlineProfileScope& session);
	};
}

#ifndef NDEBUG
#define AQUA_PROFILE 1
#else
#define AQUA_PROFILE 0
#endif

#if AQUA_PROFILE
#if defined(__GNUC__) || (defined(__MWERKS__) && (__MWERKS__ >= 0x3000)) || (defined(__ICC) && (__ICC >= 600)) || defined(__ghs__)
#define AQUA_FUNC_SIG __PRETTY_FUNCTION__
#elif defined(__DMC__) && (__DMC__ >= 0x810)
#define AQUA_FUNC_SIG __PRETTY_FUNCTION__
#elif (defined(__FUNCSIG__) || (_MSC_VER))
#define AQUA_FUNC_SIG __FUNCSIG__
#elif (defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 600)) || (defined(__IBMCPP__) && (__IBMCPP__ >= 500))
#define AQUA_FUNC_SIG __FUNCTION__
#elif defined(__BORLANDC__) && (__BORLANDC__ >= 0x550)
#define AQUA_FUNC_SIG __FUNC__
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
#define AQUA_FUNC_SIG __func__
#elif defined(__cplusplus) && (__cplusplus >= 201103)
#define AQUA_FUNC_SIG __func__
#else
#define AQUA_FUNC_SIG "MAD_FUNC_SIG unknown!"
#endif

#define PROFILE_BEGIN(filename) AquaEngine::InlineProfiler::Start(filename)
#define PROFILE_END() AquaEngine::InlineProfiler::End()
#define PROFILE_SCOPE_LINE2(name, line) auto scope##line = MadEngine::InlineProfiler::InlineProfileScope(string(name));
#define PROFILE_SCOPE_LINE(name, line) PROFILE_SCOPE_LINE2(name, line)
#define PROFILE_SCOPE(name) PROFILE_SCOPE_LINE(name, __LINE__)
#define PROFILE_FN() PROFILE_SCOPE(AQUA_FUNC_SIG)
#else
#define PROFILE_BEGIN(filename)
#define PROFILE_END()
#define PROFILE_SCOPE_LINE(name, line)
#define PROFILE_SCOPE(name)
#define PROFILE_FN()
#endif