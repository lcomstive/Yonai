#include <spdlog/spdlog.h>
#include <Yonai/Scripting/Method.hpp>

using namespace Yonai::Scripting;

Method::Method(MonoMethod* handle) { Handle = handle; }

MonoObject* Method::Invoke(MonoObject* instance, void** params)
{
	if (!Handle)
	{
		spdlog::warn("Tried to invoke method but no valid one was passed");
		return nullptr;
	}
	MonoObject* exception = nullptr;
	MonoObject* output = mono_runtime_invoke(Handle, instance, params, &exception);

	if (exception)
		mono_print_unhandled_exception(exception);
	return exception ? nullptr : output;
}