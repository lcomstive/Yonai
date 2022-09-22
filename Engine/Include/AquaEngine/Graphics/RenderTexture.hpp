#pragma once
#include <glm/glm.hpp>
#include <AquaEngine/API.hpp>

namespace AquaEngine::Graphics
{
	enum class TextureFormat
	{
		None = 0,

		// Colour
		RGBA8,
		RGBA16,
		RGBA16F,
		RedInteger,

		Cubemap,

		// Depth & Stencil
		Depth24Stencil8,

		// Aliases
		Depth = Depth24Stencil8
	};

	AquaAPI bool IsDepthFormat(TextureFormat format);
	AquaAPI GLenum GetTextureTarget(TextureFormat format, bool multisampled = false);
	AquaAPI GLenum TextureFormatToGLFormat(TextureFormat format);
	AquaAPI GLenum TextureFormatToInternalGLFormat(TextureFormat format);

	class RenderTexture
	{
		glm::ivec2 m_Resolution;
		unsigned int m_ID, m_Samples = 1;
		TextureFormat m_Format = TextureFormat::RGBA8;

		void Recreate();

		void CreateDepthTexture();
		void CreateColourTexture();

	public:
		RenderTexture(
			glm::ivec2 resolution,
			TextureFormat format = TextureFormat::RGBA8,
			unsigned int samples = 1);
		~RenderTexture();

		AquaAPI unsigned int GetID();

		AquaAPI void Bind(unsigned int textureIndex = 0);
		AquaAPI void Unbind();

		AquaAPI void CopyTo(RenderTexture* destination);

		AquaAPI unsigned int GetSamples();
		AquaAPI void SetSamples(unsigned int samples);

		AquaAPI TextureFormat GetFormat();
		AquaAPI void SetFormat(TextureFormat format);

		AquaAPI glm::ivec2 GetResolution();
		AquaAPI void SetResolution(glm::ivec2 newResolution);
	};
}