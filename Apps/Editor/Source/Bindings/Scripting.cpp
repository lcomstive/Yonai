#include <AquaEngine/Scripting/ScriptEngine.hpp>
#include <AquaEngine/Scripting/InternalCalls.hpp>

using namespace AquaEngine::Scripting;

ADD_MANAGED_METHOD(Scripting, Reload, void, (), AquaEditor)
{ ScriptEngine::SetAwaitingReload(); }

ADD_MANAGED_METHOD(Scripting, _DebuggingEnabled, bool, (), AquaEditor)
{ return ScriptEngine::DebuggingEnabled(); }

ADD_MANAGED_METHOD(Scripting, LoadAssembly, MonoAssembly*, (MonoString* pathRaw, bool shouldWatch), AquaEditor)
{
	char* path = mono_string_to_utf8(pathRaw);
	Assembly* assembly = ScriptEngine::LoadAssembly(std::string(path), shouldWatch);
	mono_free(path);
	return assembly->Handle;
}

ADD_MANAGED_METHOD(Scripting, IsAssemblyLoaded, bool, (MonoString* pathRaw), AquaEditor)
{
	char* path = mono_string_to_utf8(pathRaw);
	bool loaded = ScriptEngine::IsAssemblyLoaded(path);
	mono_free(path);
	return loaded;
}