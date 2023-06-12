#pragma once
#include <glm/glm.hpp>
#include <AquaEngine/Graphics/Shader.hpp>
#include <AquaEngine/Graphics/Framebuffer.hpp>
#include <AquaEngine/Components/Component.hpp>
#include <AquaEngine/Components/Transform.hpp>

namespace AquaEngine::Components
{
	struct Camera : public Component
	{
		float FieldOfView = 60.0f;
		float Near = 0.1f;
		float Far = 1000.0f;
		bool Orthographic = false;
		float OrthographicSize = 5.0f;

		Graphics::RenderTexture* RenderTarget = nullptr;

		AquaAPI glm::mat4 GetViewMatrix();
		AquaAPI glm::mat4 GetProjectionMatrix(glm::ivec2 resolution);
		AquaAPI glm::mat4 GetProjectionMatrix(int resolutionWidth, int resolutionHeight);

		AquaAPI void FillShader(Graphics::Shader* shader, glm::ivec2 resolution);

	private:
		static Camera* s_MainCamera;
	};
}
