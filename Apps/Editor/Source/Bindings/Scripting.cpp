#include <Yonai/SystemManager.hpp>
#include <Yonai/Scripting/ScriptEngine.hpp>
#include <Yonai/Scripting/InternalCalls.hpp>

using namespace Yonai;
using namespace Yonai::Scripting;

ADD_MANAGED_METHOD(Scripting, Reload, void, (), YonaiEditor)
{ ScriptEngine::SetAwaitingReload(); }

ADD_MANAGED_METHOD(Scripting, _DebuggingEnabled, bool, (), YonaiEditor)
{ return ScriptEngine::DebuggingEnabled(); }

ADD_MANAGED_METHOD(Scripting, LoadAssembly, MonoAssembly*, (MonoString* pathRaw, bool shouldWatch), YonaiEditor)
{
	char* path = mono_string_to_utf8(pathRaw);
	Assembly* assembly = ScriptEngine::LoadAssembly(std::string(path), shouldWatch);
	mono_free(path);
	return assembly->Handle;
}

ADD_MANAGED_METHOD(Scripting, IsAssemblyLoaded, bool, (MonoString* pathRaw), YonaiEditor)
{
	char* path = mono_string_to_utf8(pathRaw);
	bool loaded = ScriptEngine::IsAssemblyLoaded(path);
	mono_free(path);
	return loaded;
}

ADD_MANAGED_METHOD(Scripting, IsAssemblyReloading, bool, (), YonaiEditor)
{ return ScriptEngine::IsReloading(); }

ADD_MANAGED_METHOD(Scripting, _GlobalSystemManagerShouldUpdate, void, (bool shouldUpdate), YonaiEditor)
{ SystemManager::Global()->ShouldUpdate = shouldUpdate; }
