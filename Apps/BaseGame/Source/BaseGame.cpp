#include <BaseGame.hpp>
#include <AquaEngine/Window.hpp>
#include <AquaEngine/Scripting/ScriptEngine.hpp>
#include <AquaEngine/Scripting/InternalCalls.hpp>
#include <AquaEngine/Systems/Global/RenderSystem.hpp>

using namespace std;
using namespace AquaEngine;
using namespace AquaEngine::IO;
using namespace AquaEngine::Systems;
using namespace AquaEngine::Scripting;

string AssembliesPath = "./Assets/Editor/Mono/";

bool LoadAssembly(MonoArray*, MonoString*);

BaseGame::BaseGame(int argc, char** argv) : WindowedApplication(argc, argv)
{ InitialiseScripting(); }

void BaseGame::Setup()
{
	SystemManager::Global()->Add<SceneSystem>();
	m_RenderSystem = SystemManager::Global()->Add<RenderSystem>();

	// Call AquaEngine.Application.BaseGameLaunch()
	MonoClass* appClass = ScriptEngine::GetCoreAssembly()->GetClassFromName("AquaEngine", "BaseGameLauncher");
	MonoMethod* method = mono_class_get_method_from_name(appClass, "Launch", 0);
	mono_runtime_invoke(method, nullptr, nullptr, nullptr);
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
}

bool LoadAssembly(MonoArray* dataRaw, MonoString* friendlyNameRaw)
{
	// Get data
	vector<unsigned char> data;
	data.resize(mono_array_length(dataRaw));
	for (size_t i = 0; i < data.size(); i++)
		data[i] = mono_array_get(dataRaw, unsigned char, i);

	// Get friendly name
	char* friendlyName = mono_string_to_utf8(friendlyNameRaw);

	Assembly* assembly = ScriptEngine::LoadAssembly(data, friendlyName);
	
	mono_free(friendlyName);
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
	m_RenderSystem->GetPipeline()->GetOutput()->BlitTo();
}

void BaseGame::OnPreDraw()
{
	m_RenderSystem->GetPipeline()->SetResolution(Window::GetResolution());
}