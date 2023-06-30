#include <BaseGame.hpp>
#include <AquaEngine/Window.hpp>
#include <AquaEngine/Scripting/ScriptEngine.hpp>
#include <AquaEngine/Scripting/InternalCalls.hpp>

using namespace std;
using namespace AquaEngine;
using namespace AquaEngine::IO;
using namespace AquaEngine::Scripting;

string AssembliesPath = "./Assets/Editor/Mono/";

bool LoadAssembly(MonoString*);

BaseGame::BaseGame(int argc, char** argv) : WindowedApplication(argc, argv)
{
	/*
	VFS::Mount("build://", "./Assets");
	VFS::Mount("mono://", "build://Editor/Mono");
	VFS::Mount("project://", "build://ProjectFiles");
	*/
	
	InitialiseScripting();

	auto systems = SystemManager::Global()->All();
}

void BaseGame::InitialiseScripting()
{
	ScriptEngine::Init(AssembliesPath,
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

#include <vector>
#include <AquaEngine/Components/Camera.hpp>
#include <AquaEngine/Systems/Global/SceneSystem.hpp>
using namespace AquaEngine::Systems;
using namespace AquaEngine::Components;
void BaseGame::OnPostDraw()
{
	// Blit output to default framebuffer
	SystemManager::Global()->Get<RenderSystem>()->GetPipeline()->GetOutput()->BlitTo();
}