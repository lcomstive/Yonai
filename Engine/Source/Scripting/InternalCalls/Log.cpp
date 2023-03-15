#include <mono/jit/jit.h>
#include <spdlog/spdlog.h>
#include <AquaEngine/Scripting/Assembly.hpp>
#include <AquaEngine/Scripting/InternalCalls.hpp>

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
ADD_MANAGED_METHOD(Log, NativeLog, void, (MonoString* rawMsg, int level))
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
