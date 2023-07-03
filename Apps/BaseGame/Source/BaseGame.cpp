#include <BaseGame.hpp>
#include <AquaEngine/Window.hpp>
#include <AquaEngine/Scripting/ScriptEngine.hpp>
#include <AquaEngine/Systems/Global/RenderSystem.hpp>

#include <AquaEngine/Scripting/Class.hpp>

using namespace std;
using namespace AquaEngine;
using namespace AquaEngine::IO;
using namespace AquaEngine::Systems;
using namespace AquaEngine::Scripting;

string AssembliesPath = "/Assets/Editor/Mono";

bool LoadAssembly(MonoArray*, MonoString*);

BaseGame::BaseGame(int argc, char** argv) : WindowedApplication(argc, argv)
{ InitialiseScripting(); }

void BaseGame::Setup()
{
	SystemManager::Global()->Add<SceneSystem>();
	m_RenderSystem = SystemManager::Global()->Add<RenderSystem>();

	glm::vec2 resolution = Window::GetResolution();
	glm::vec2 scaling = Window::GetContentScaling();
	resolution.x *= scaling.x;
	resolution.y *= scaling.y;
	m_RenderSystem->GetPipeline()->SetResolution(resolution);

	// Call AquaEngine.Application.BaseGameLaunch()
	Scripting::Class appClass(ScriptEngine::GetCoreAssembly()->GetClassFromName("AquaEngine", "BaseGameLauncher"), nullptr);
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

#include <vector>
#include <AquaEngine/Components/Camera.hpp>
#include <AquaEngine/Systems/Global/SceneSystem.hpp>
using namespace AquaEngine::Systems;
using namespace AquaEngine::Components;
void BaseGame::OnPostDraw()
{
	// Blit output to default framebuffer
	m_RenderSystem->GetPipeline()->GetOutput()->BlitTo();
}

void BaseGame::OnPreDraw()
{
	m_RenderSystem->GetPipeline()->SetResolution(Window::GetResolution());
}