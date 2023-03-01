#pragma once
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <AquaEngine/Resource.hpp>
#include <AquaEngine/Application.hpp>

namespace AquaEditor
{
	class SplashScreenApp : public AquaEngine::Application
	{
		const glm::ivec2 SplashResolution = { 600, 600 };
		const char* SplashImagePath = "assets://Textures/Splash.png";

		GLFWwindow* m_Window;
		bool m_LaunchDisrupted = false;
		AquaEngine::ResourceID m_Texture, m_Shader, m_QuadMesh;

	protected:
		void OnUpdate() override;

	public:
		void Setup() override;
		void Cleanup() override;

		/// <returns>False if failed to load, or if user requested closing</returns>
		bool LaunchDisrupted();
	};
}