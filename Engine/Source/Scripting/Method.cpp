#include <spdlog/spdlog.h>
#include <AquaEngine/Scripting/Method.hpp>

using namespace AquaEngine::Scripting;

Method::Method(MonoMethod* handle) { Handle = handle; }

void Method::Invoke(MonoObject* instance, void** params)
{
	if (!Handle)
	{
		spdlog::warn("Tried to invoke method but no valid one was passed");
		return;
	}
	MonoObject* exception = nullptr;
	mono_runtime_invoke(Handle, instance, params, &exception);

	if (exception)
		mono_print_unhandled_exception(exception);
}