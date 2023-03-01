#include <SpashScreen.hpp>
#include <AquaEngine/Time.hpp>
#include <AquaEngine/IO/VFS.hpp>
#include <AquaEngine/Graphics/Shader.hpp>
#include <AquaEngine/Graphics/Texture.hpp>
#include <AquaEngine/Systems/Global/SceneSystem.hpp>
#include <AquaEngine/Systems/Global/RenderSystem.hpp>

using namespace AquaEngine;
using namespace AquaEditor;
using namespace AquaEngine::Systems;
using namespace AquaEngine::Graphics;

void SplashScreenApp::Setup()
{
	IO::VFS::Mount("assets://", "app://Assets");
	Application::Setup();

	// Initialise window
	if (glfwInit() == GLFW_FALSE)
	{
		spdlog::critical("Failed to initialise GLFW");
		return;
	}

	glfwDefaultWindowHints();
	glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);

	// Create window
	m_Window = glfwCreateWindow(SplashResolution.x, SplashResolution.y, "", nullptr, nullptr);
	glfwMakeContextCurrent(m_Window);

	// Initialise OpenGL
	gladLoadGL(glfwGetProcAddress);

	// Hide titlebar of window
	glfwSetWindowAttrib(m_Window, GLFW_DECORATED, GLFW_FALSE);

	// Center window
	const GLFWvidmode* videomode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	glfwSetWindowPos(m_Window, videomode->width / 2 - SplashResolution.x / 2, videomode->height / 2 - SplashResolution.y / 2);

	// Load resources
	m_Texture = Resource::Load<Texture>("Texture/Editor/SplashScreen", SplashImagePath);

	m_Shader = Resource::Load<Shader>("Shader/BasicSprite", ShaderStageInfo
		{
			"assets://Shaders/Fullscreen.vert",
			"assets://Shaders/SplashScreen.frag"
		});

	m_QuadMesh = Mesh::Quad();
}

void SplashScreenApp::Cleanup()
{
	// Release resources
	Resource::Unload(m_Shader);
	Resource::Unload(m_Texture);

	// Close GLFW
	glfwTerminate();
}

void SplashScreenApp::OnUpdate()
{
	// Clear framebuffer
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	Mesh* mesh = Resource::Get<Mesh>(m_QuadMesh);
	Shader* shader = Resource::Get<Shader>(m_Shader);
	Texture* texture = Resource::Get<Texture>(m_Texture);

	shader->Bind();

	// Bind texture to index 0 and inform shader
	texture->Bind();
	shader->Set("inputTexture", 0);
	shader->Set("time", Time::SinceLaunch());

	mesh->Draw();

	shader->Unbind();

	glfwSwapBuffers(m_Window);
	glfwPollEvents();

	// Check if user closing window
	if (glfwWindowShouldClose(m_Window))
	{
		m_LaunchDisrupted = true;
		Exit();
		return;
	}

#pragma region Debugging
	// Check if time is up
	const float SplashTime = 10; // seconds
	if (Time::SinceLaunch() > SplashTime)
		Exit();
#pragma endregion
}

bool SplashScreenApp::LaunchDisrupted() { return m_LaunchDisrupted; }