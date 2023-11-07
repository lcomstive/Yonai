#pragma once
#include <glm/glm.hpp>
#include <Yonai/API.hpp>

namespace Yonai::Graphics
{
	enum class TextureFormat : unsigned char
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

	YonaiAPI bool IsDepthFormat(TextureFormat format);

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

		YonaiAPI unsigned int GetID();

		YonaiAPI void Bind(unsigned int textureIndex = 0);
		YonaiAPI void Unbind();

		YonaiAPI void CopyTo(RenderTexture* destination);

		YonaiAPI unsigned int GetSamples();
		YonaiAPI void SetSamples(unsigned int samples);

		YonaiAPI TextureFormat GetFormat();
		YonaiAPI void SetFormat(TextureFormat format);

		YonaiAPI glm::ivec2 GetResolution();
		YonaiAPI void SetResolution(glm::ivec2 newResolution);
	};
}