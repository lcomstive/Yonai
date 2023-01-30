#include <memory>
#include <unordered_map>
#include <mono/jit/jit.h>
#include <spdlog/spdlog.h>
#include <AquaEngine/SystemManager.hpp>
#include <AquaEngine/Systems/System.hpp>
#include <AquaEngine/Scripting/Assembly.hpp>
#include <AquaEngine/Scripting/UnmanagedThunks.hpp>

using namespace std;
using namespace AquaEngine;
using namespace AquaEngine::Systems;
using namespace AquaEngine::Scripting;

extern EmptyMethodFn SystemMethodStart;
extern EmptyMethodFn SystemMethodUpdate;
extern EmptyMethodFn SystemMethodDestroyed;
extern SystemMethodEnabledFn SystemMethodEnabled;
extern SystemMethodInitialiseFn SystemMethodInitialise;

bool Exists(unsigned int worldID, MonoReflectionType* componentType)
{
	return false;
}

#define ADD_SYSTEM_INTERNAL_CALL(name) mono_add_internal_call("AquaEngine.System::_aqua_internal_"#name, (const void*)name);

void AquaEngine::Scripting::Assembly::AddSystemInternalCalls()
{
	ADD_SYSTEM_INTERNAL_CALL(Exists)
}