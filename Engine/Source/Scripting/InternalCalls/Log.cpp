#include <mono/jit/jit.h>
#include <spdlog/spdlog.h>

namespace InternalCalls
{
	/// <summary>
	/// Levels:
	/// default   - debug
	///			1 - info
	///			2 - warning
	///			3 - error
	///			4 - critical
	/// </summary>
	/// <param name="msg"></param>
	/// <param name="level"></param>
	void NativeLog(MonoString* rawMsg, int level)
	{
		char* msg = mono_string_to_utf8(rawMsg);
		switch (level)
		{
		default: spdlog::debug(msg);	break;
		case 1: spdlog::info(msg);		break;
		case 2: spdlog::warn(msg);		break;
		case 3: spdlog::error(msg);		break;
		case 4: spdlog::critical(msg);	break;
		}
		mono_free(msg);
	}

	void AddLogInternalCalls()
	{
		mono_add_internal_call("AquaEngine.Log::_aqua_internal_NativeLog", (const void*)NativeLog);
	}
}