#include <spdlog/spdlog.h>
#include <AquaEngine/Scripting/Method.hpp>

using namespace AquaEngine::Scripting;

void Method::Invoke(void** params)
{
	if (!Handle)
	{
		spdlog::warn("Trying to invoke null method?");
		return;
	}
	MonoObject* exception = nullptr;
	mono_runtime_invoke(Handle, ClassInstance, nullptr, &exception);
}

bool Method::IsValid() { return Handle != nullptr; }