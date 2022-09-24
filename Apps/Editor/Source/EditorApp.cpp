#include <EditorApp.hpp>
#include <spdlog/spdlog.h>
#include <AquaEngine/Window.hpp>
#include <AquaEngine/IO/VFS.hpp>

using namespace AquaEditor;
using namespace AquaEngine;
using namespace AquaEngine::IO;

void EditorApp::Setup()
{
	Window::SetTitle("Aqua Editor");

	if(HasArg("ProjectPath"))
		m_ProjectPath = GetArg("ProjectPath");
	else
		m_ProjectPath = "./";
	spdlog::info("Project path: {}", m_ProjectPath);

	VFS::Mount("/Assets", m_ProjectPath);
}
