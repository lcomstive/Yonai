#pragma once
#include <glm/glm.hpp>
#include <Yonai/Graphics/Shader.hpp>
#include <Yonai/Graphics/Framebuffer.hpp>
#include <Yonai/Components/Component.hpp>
#include <Yonai/Components/Transform.hpp>

namespace Yonai::Components
{
	struct Camera : public Component
	{
		float FieldOfView = 60.0f;
		float Near = 0.1f;
		float Far = 1000.0f;
		bool Orthographic = false;
		float OrthographicSize = 5.0f;

		Graphics::RenderTexture* RenderTarget = nullptr;

		YonaiAPI glm::mat4 GetViewMatrix();
		YonaiAPI glm::mat4 GetProjectionMatrix(glm::ivec2 resolution);
		YonaiAPI glm::mat4 GetProjectionMatrix(int resolutionWidth, int resolutionHeight);

		YonaiAPI void FillShader(Graphics::Shader* shader, glm::ivec2 resolution);

	private:
		static Camera* s_MainCamera;
	};
}
