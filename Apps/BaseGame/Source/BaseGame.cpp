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

string AssembliesPath = "/Assets/Editor/Mono";

bool LoadAssembly(MonoArray*, MonoString*);

BaseGame::BaseGame(int argc, char** argv) : WindowedApplication(argc, argv)
{ InitialiseScripting(); }

void BaseGame::Setup()
{
	Window::Show(false);

	SystemManager::Global()->Add<SceneSystem>();

	glm::vec2 resolution = Window::GetResolution();
	glm::vec2 scaling = Window::GetContentScaling();
	resolution.x *= scaling.x;
	resolution.y *= scaling.y;

	// Launch BaseGameLauncher
	Scripting::Class appClass(ScriptEngine::GetCoreAssembly()->GetClassFromName("Yonai", "BaseGameLauncher"), nullptr);
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
