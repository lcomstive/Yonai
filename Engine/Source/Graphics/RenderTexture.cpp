#include <spdlog/spdlog.h>
#include <Yonai/Graphics/RenderTexture.hpp>

using namespace glm;
using namespace Yonai;
using namespace Yonai::Graphics;

bool Yonai::Graphics::IsDepthFormat(TextureFormat format)
{
	switch (format)
	{
	default: return false;
	case TextureFormat::Depth: return true;
	}
}

RenderTexture::RenderTexture(ivec2 resolution, TextureFormat format, unsigned int samples) : m_ID(0)
{
	m_Format = format;
	m_Samples = samples;
	m_Resolution = resolution;

	Recreate();
}

RenderTexture::~RenderTexture() {}

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
}

void RenderTexture::Unbind() {}

void RenderTexture::CreateDepthTexture() {}

void RenderTexture::CreateColourTexture() {}

void RenderTexture::CopyTo(RenderTexture* destination)
{
	ivec2 outputResolution =
	{
		min(m_Resolution.x, destination->m_Resolution.x),
		min(m_Resolution.y, destination->m_Resolution.y)
	};

	
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