#include <AquaEngine/Scripting/ScriptEngine.hpp>
#include <AquaEngine/Scripting/InternalCalls.hpp>

using namespace AquaEngine::Scripting;

ADD_MANAGED_METHOD(Scripting, Reload, void, (), AquaEditor)
{ ScriptEngine::SetAwaitingReload(); }

ADD_MANAGED_METHOD(Scripting, _DebuggingEnabled, bool, (), AquaEditor)
{ return ScriptEngine::DebuggingEnabled(); }
