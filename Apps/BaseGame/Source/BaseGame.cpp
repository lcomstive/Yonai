#include <BaseGame.hpp>
#include <AquaEngine/IO/VFS.hpp>
#include <AquaEngine/Scripting/ScriptEngine.hpp>
#include <AquaEngine/Scripting/InternalCalls.hpp>

using namespace std;
using namespace AquaEngine;
using namespace AquaEngine::IO;
using namespace AquaEngine::Scripting;

string AquaScriptCorePath = "mono://AquaScriptCore.dll";

bool LoadAssembly(MonoString*);

void BaseGame::Setup()
{
	WindowedApplication::Setup();

	VFS::Mount("build://", "./Assets");
	VFS::Mount("mono://", "build://Mono");
	VFS::Mount("project://", "build://ProjectFiles");
	
	InitialiseScripting();

	auto systems = SystemManager::Global()->All();
}

void BaseGame::InitialiseScripting()
{
	if (AquaScriptCorePath.empty() || !VFS::Exists(AquaScriptCorePath))
	{
		spdlog::critical("Core DLL path not specified or file '{}' does not exist.", AquaScriptCorePath.c_str());
		Exit();
		return;
	}

	ScriptEngine::Init(AquaScriptCorePath,
//#ifndef NDEBUG // Allow debugging in debug builds
		true
		/*
#else // Release mode
		false
#endif
		*/
	);

	ScriptEngine::AddInternalCall("AquaEngine.BaseGameLauncher::_LoadAssembly", (const void*)LoadAssembly);

	// Call AquaEngine.Application.BaseGameLaunch()
	MonoClass* appClass = ScriptEngine::GetCoreAssembly()->GetClassFromName("AquaEngine", "BaseGameLauncher");
	MonoMethod* method = mono_class_get_method_from_name(appClass, "Launch", 0);
	mono_runtime_invoke(method, nullptr, nullptr, nullptr);
}

bool LoadAssembly(MonoString* pathRaw)
{
	char* path = mono_string_to_utf8(pathRaw);
	Assembly* assembly = ScriptEngine::LoadAssembly(path);
	mono_free(path);
	return assembly != nullptr;
}