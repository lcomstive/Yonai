#include <BaseGame.hpp>
#include <Yonai/Window.hpp>
#include <Yonai/Scripting/ScriptEngine.hpp>
#include <Yonai/Systems/Global/SceneSystem.hpp>

#include <Yonai/Scripting/Class.hpp>

using namespace std;
using namespace Yonai;
using namespace Yonai::IO;
using namespace Yonai::Systems;
using namespace Yonai::Scripting;

typedef void (*PostDrawFn)(MonoObject*, MonoException**);
PostDrawFn PostDrawFunction;

string AssembliesPath = "/Assets/Editor/Mono";

bool LoadAssembly(MonoArray*, MonoString*);

BaseGame::BaseGame(int argc, char** argv) : WindowedApplication(argc, argv)
{ InitialiseScripting(); }

void BaseGame::Setup()
{
	// Add global systems
	SystemManager::Global()->Add<SceneSystem>();

	Scripting::Class appClass(ScriptEngine::GetCoreAssembly()->GetClassFromName("Yonai", "BaseGameLauncher"), nullptr);

	// Get post draw thunk
	MonoMethod* postDrawMethod = mono_class_get_method_from_name(appClass.Handle, "_PostDraw", 0);
	PostDrawFunction = (PostDrawFn)mono_method_get_unmanaged_thunk(postDrawMethod);

	// Launch BaseGameLauncher
	Method method = appClass.GetMethod("Launch", 0);
	method.Invoke();
}

void BaseGame::InitialiseScripting()
{
	ScriptEngine::Init(GetExecutableDirectory().string() + AssembliesPath,
// #ifndef NDEBUG // Allow debugging in debug builds
		true
/*
#else // Release mode
		false
#endif
*/
	);
}

void BaseGame::OnPostDraw()
{
	MonoException* exception;
	PostDrawFunction(nullptr, &exception);
}
