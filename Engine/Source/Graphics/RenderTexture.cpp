#include <glad/glad.h>
#include <spdlog/spdlog.h>
#include <Yonai/Graphics/RenderTexture.hpp>

using namespace glm;
using namespace Yonai;
using namespace Yonai::Graphics;

GLenum Yonai::Graphics::GetTextureTarget(TextureFormat target, bool multisampled)
{
	switch (target)
	{
	default:
	case TextureFormat::None:
		return GL_INVALID_ENUM;
	case TextureFormat::RedInteger:
		return GL_TEXTURE_1D;
	case TextureFormat::RGBA8:
	case TextureFormat::RGBA16:
	case TextureFormat::RGBA16F:
		return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
	case TextureFormat::Cubemap:
		return GL_TEXTURE_CUBE_MAP;
	case TextureFormat::Depth:
		return GL_TEXTURE_DEPTH;
	}
}

GLenum Yonai::Graphics::TextureFormatToInternalGLFormat(TextureFormat format)
{
	switch (format)
	{
	default: return GL_INVALID_ENUM;
	case TextureFormat::RGBA8:		return GL_RGBA8;
	case TextureFormat::RGBA16:		return GL_RGBA16;
	case TextureFormat::RGBA16F:	return GL_RGBA16F;
	case TextureFormat::RedInteger:	return GL_R32I;
	case TextureFormat::Cubemap:	return GL_RGB16F;
	}
}

GLenum Yonai::Graphics::TextureFormatToGLFormat(TextureFormat format)
{
	switch (format)
	{
	default: return GL_INVALID_ENUM;
	case TextureFormat::RGBA8:
	case TextureFormat::RGBA16F:		 return GL_RGBA;
	case TextureFormat::RedInteger:		 return GL_RED_INTEGER;
	case TextureFormat::Depth24Stencil8: return GL_DEPTH24_STENCIL8;
	case TextureFormat::Cubemap:		 return GL_RGB;
	}
}

bool Yonai::Graphics::IsDepthFormat(TextureFormat format)
{
	switch (format)
	{
	default: return false;
	case TextureFormat::Depth: return true;
	}
}

RenderTexture::RenderTexture(ivec2 resolution, TextureFormat format, unsigned int samples) : m_ID(GL_INVALID_VALUE)
{
	m_Format = format;
	m_Samples = samples;
	m_Resolution = resolution;

	Recreate();
}

RenderTexture::~RenderTexture()
{
	if (m_ID != GL_INVALID_VALUE)
		glDeleteTextures(1, &m_ID);
	m_ID = GL_INVALID_VALUE;
}

void RenderTexture::SetResolution(ivec2 resolution)
{
	if (m_Resolution == resolution)
		return;

	m_Resolution = resolution;
	Recreate();
}

void RenderTexture::SetSamples(unsigned int samples)
{
	if (samples == m_Samples)
		return;

	m_Samples = samples;
	Recreate();
}

void RenderTexture::SetFormat(TextureFormat format)
{
	if (format == m_Format)
		return;

	if (IsDepthFormat(m_Format))
		glDeleteRenderbuffers(1, &m_ID);
	else
		glDeleteTextures(1, &m_ID);

	// Create in new format
	m_Format = format;
	Recreate();
}

unsigned int RenderTexture::GetID() { return m_ID; }
ivec2 RenderTexture::GetResolution() { return m_Resolution; }
TextureFormat RenderTexture::GetFormat() { return m_Format; }
unsigned int RenderTexture::GetSamples() { return m_Samples; }

void RenderTexture::Recreate()
{
	if (IsDepthFormat(m_Format))
		CreateDepthTexture();
	else
		CreateColourTexture();
}

void RenderTexture::Bind(unsigned int textureIndex)
{
	if (!IsDepthFormat(m_Format))
	{
		glActiveTexture(GL_TEXTURE0 + textureIndex);
		glBindTexture(GetTextureTarget(m_Format, m_Samples > 1), m_ID);
	}
	else
		glBindRenderbuffer(GL_RENDERBUFFER, m_ID);		
}

void RenderTexture::Unbind()
{
	if (IsDepthFormat(m_Format))
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	else
		glBindTexture(GetTextureTarget(m_Format, m_Samples > 1), 0);
}

void RenderTexture::CreateDepthTexture()
{
	if (m_ID == GL_INVALID_VALUE)
		glGenRenderbuffers(1, &m_ID);

	glBindRenderbuffer(GL_RENDERBUFFER, m_ID);

	if (m_Samples <= 1)
		glRenderbufferStorage(
			GL_RENDERBUFFER,
			TextureFormatToGLFormat(m_Format),
			m_Resolution.x,
			m_Resolution.y
		);
	else
		glRenderbufferStorageMultisample(
			GL_RENDERBUFFER,
			m_Samples,
			TextureFormatToGLFormat(m_Format),
			m_Resolution.x,
			m_Resolution.y
		);

	// Unbind
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void RenderTexture::CreateColourTexture()
{
	if (m_ID == GL_INVALID_VALUE)
		glGenTextures(1, &m_ID);

	GLenum textureTarget = GetTextureTarget(m_Format, m_Samples > 1);
	GLenum GLFormat = TextureFormatToGLFormat(m_Format);
	GLenum internalGLFormat = TextureFormatToInternalGLFormat(m_Format);
	Bind();

	switch (m_Format)
	{
	case TextureFormat::None:
	case TextureFormat:: RedInteger:
	case TextureFormat::Depth24Stencil8:
		// TODO: Properly implement these
		spdlog::warn("The '{}' texture format is not currently implemented!", (unsigned char)m_Format);
		break;
	case TextureFormat::RGBA8:
	case TextureFormat::RGBA16:
	case TextureFormat::RGBA16F:
		if (m_Samples <= 1)
		{
			glTexImage2D(
				GL_TEXTURE_2D,		// Target
				0,					// Mipmap Level
				internalGLFormat,	// Internal Format
				m_Resolution.x,		// Resolution
				m_Resolution.y,
				0,					// Border
				GLFormat,			// Format
				GL_UNSIGNED_BYTE,	// Type
				nullptr				// Data/Pixels
			);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}
		else
			glTexImage2DMultisample(
				GL_TEXTURE_2D_MULTISAMPLE,
				m_Samples,
				internalGLFormat,
				m_Resolution.x,
				m_Resolution.y,
				GL_TRUE				// Fixed sample locations
			);
		break;
	case TextureFormat::Cubemap:
		for (unsigned int i = 0; i < 6; i++)
			glTexImage2D(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, // Mip level
				internalGLFormat,
				m_Resolution.x,
				m_Resolution.y,
				0, // Border
				GLFormat,
				GL_FLOAT,
				nullptr);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		break;
	}

	Unbind();
}

void RenderTexture::CopyTo(RenderTexture* destination)
{
	ivec2 outputResolution =
	{
		min(m_Resolution.x, destination->m_Resolution.x),
		min(m_Resolution.y, destination->m_Resolution.y)
	};

	glCopyImageSubData(
		m_ID,
		GetTextureTarget(m_Format, m_Samples > 1),
		0,	// Mip level
		0, 0, 0,

		destination->m_ID,
		GetTextureTarget(destination->m_Format, destination->m_Samples > 1),
		0,	// Mip level
		0, 0, 0,

		outputResolution.x,
		outputResolution.y,
		1 // Source Depth
	);
}

#pragma region Internal Calls
#include <Yonai/Scripting/InternalCalls.hpp>

ADD_MANAGED_METHOD(RenderTexture, Load, void*, (glm::ivec2* resolution, unsigned char textureFormat, unsigned int samples), Yonai.Graphics)
{ return new RenderTexture(*resolution, (TextureFormat)textureFormat, samples); }

ADD_MANAGED_METHOD(RenderTexture, Destroy, void, (void* handle), Yonai.Graphics)
{ if(handle) delete handle; }

ADD_MANAGED_METHOD(RenderTexture, Bind, void, (void* handle, unsigned int index), Yonai.Graphics)
{ ((RenderTexture*)handle)->Bind(index); }

ADD_MANAGED_METHOD(RenderTexture, Unbind, void, (void* handle), Yonai.Graphics)
{ ((RenderTexture*)handle)->Unbind(); }

ADD_MANAGED_METHOD(RenderTexture, SetFormat, void, (void* handle, unsigned char format), Yonai.Graphics)
{ ((RenderTexture*)handle)->SetFormat((TextureFormat)format); }

ADD_MANAGED_METHOD(RenderTexture, SetSamples, void, (void* handle, unsigned int samples), Yonai.Graphics)
{ ((RenderTexture*)handle)->SetSamples(samples); }

ADD_MANAGED_METHOD(RenderTexture, SetResolution, void, (void* handle, glm::ivec2* resolution), Yonai.Graphics)
{ ((RenderTexture*)handle)->SetResolution(*resolution); }

ADD_MANAGED_METHOD(RenderTexture, GetFormat, unsigned char, (void* handle), Yonai.Graphics)
{ return (unsigned char)((RenderTexture*)handle)->GetFormat(); }

ADD_MANAGED_METHOD(RenderTexture, GetSamples, unsigned int, (void* handle), Yonai.Graphics)
{ return ((RenderTexture*)handle)->GetSamples(); }

ADD_MANAGED_METHOD(RenderTexture, GetResolution, void, (void* handle, glm::ivec2* resolution), Yonai.Graphics)
{ *resolution = ((RenderTexture*)handle)->GetResolution(); }

ADD_MANAGED_METHOD(RenderTexture, CopyTo, void, (void* handle, void* destination), Yonai.Graphics)
{ ((RenderTexture*)handle)->CopyTo((RenderTexture*)destination); }

#pragma endregion