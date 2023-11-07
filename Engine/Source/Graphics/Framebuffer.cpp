#include <spdlog/spdlog.h>
#include <Yonai/Resource.hpp>
#include <Yonai/Graphics/Mesh.hpp>
#include <Yonai/Graphics/Framebuffer.hpp>
#include <Yonai/Scripting/InternalCalls.hpp>

using namespace glm;
using namespace std;
using namespace Yonai;
using namespace Yonai::Graphics;

Framebuffer::Framebuffer(FramebufferSpec& specs) : m_Specs(specs), m_ID(0), m_DepthAttachment(nullptr)
{
	if (m_Specs.SwapchainTarget)
		m_ID = 0;

	Create();
}

Framebuffer::~Framebuffer() { Destroy(); }

void Framebuffer::Destroy()
{
	m_ID = 0;

	for (auto& texture : m_ColourAttachments)
		delete texture;
	m_ColourAttachments.clear();

	if (m_DepthAttachment)
		delete m_DepthAttachment;
	m_DepthAttachment = nullptr;
}

void Framebuffer::Create()
{
	if (m_Specs.SwapchainTarget)
		return;

	Bind();

	for (unsigned int i = 0; i < m_Specs.Attachments.size(); i++)
	{
		if (IsDepthFormat(m_Specs.Attachments[i]))
		{
			RenderTexture* texture = new RenderTexture(
				m_Specs.Resolution,
				m_Specs.Attachments[i],
				m_Specs.Samples
			);
			m_DepthAttachment = texture;
		}
		else
		{
			RenderTexture* texture = new RenderTexture(
				m_Specs.Resolution,
				m_Specs.Attachments[i],
				m_Specs.Samples
			);
			m_ColourAttachments.emplace_back(texture);

			
		}
	}

	

	Unbind();
}

void Framebuffer::Recreate()
{
	if (m_Specs.SwapchainTarget)
		return;

	Bind();

	// Create attachments if they don't exist, otherwise alter existing ones
	for (auto& attachment : m_ColourAttachments)
	{
		attachment->SetSamples(m_Specs.Samples);
		attachment->SetResolution(m_Specs.Resolution);
	}

	if (m_DepthAttachment)
	{
		m_DepthAttachment->SetSamples(m_Specs.Samples);
		m_DepthAttachment->SetResolution(m_Specs.Resolution);
	}
}

void Framebuffer::BlitTo(Framebuffer* other)
{
	ivec2 destRes = other ? other->m_Specs.Resolution : m_Specs.Resolution;
	
}

void Framebuffer::CopyAttachmentTo(RenderTexture* destination, unsigned int colourAttachment)
{
	Bind();
	destination->Bind();

	destination->Unbind();
	Unbind();
}

void Framebuffer::Bind() {}

void Framebuffer::Unbind() {}

void Framebuffer::SetResolution(ivec2 newResolution)
{
	m_Specs.Resolution = newResolution;
	Recreate();
}

void Framebuffer::SetSamples(unsigned int samples)
{
	m_Specs.Samples = std::clamp(samples, 1u, 32u);
	Recreate();
}

ivec2 Framebuffer::GetResolution() { return m_Specs.Resolution; }
unsigned int Framebuffer::GetSamples() { return m_Specs.Samples; }
RenderTexture* Framebuffer::GetDepthAttachment() { return m_DepthAttachment; }
RenderTexture* Framebuffer::GetColourAttachment(unsigned int index)
{
	if (m_ColourAttachments.empty())
		return nullptr;
	return m_ColourAttachments[std::clamp(index, 0u, (unsigned int)m_ColourAttachments.size() - 1)];
}

std::vector<RenderTexture*> Framebuffer::GetColourAttachments() { return m_ColourAttachments; }
std::vector<RenderTexture*> Framebuffer::GetAttachments()
{
	vector<RenderTexture*> attachments;
	attachments.insert(attachments.end(), m_ColourAttachments.begin(), m_ColourAttachments.end());
	attachments.emplace_back(m_DepthAttachment);
	return attachments;
}

unsigned int Framebuffer::AttachmentCount() { return (unsigned int)(m_ColourAttachments.size() + (m_DepthAttachment ? 0 : 1)); }
unsigned int Framebuffer::ColourAttachmentCount() { return (unsigned int)m_ColourAttachments.size(); }

bool Framebuffer::HasDepthAttachment() { return m_DepthAttachment != nullptr; }
FramebufferSpec& Framebuffer::GetSpecs() { return m_Specs; }

#pragma region Internal Calls
ADD_MANAGED_METHOD(Framebuffer, Load, void*, (glm::ivec2* resolution, bool swapchainTarget, unsigned int samples, MonoArray* attachments), Yonai.Grapghics)
{
	FramebufferSpec spec =
	{
		*resolution,
		swapchainTarget,
		samples
	};

	uintptr_t attachmentCount = mono_array_length(attachments);
	for (uintptr_t i = 0; i < attachmentCount; i++)
		spec.Attachments.push_back((TextureFormat)mono_array_get(attachments, unsigned char, i));

	return new Framebuffer(spec);
}

ADD_MANAGED_METHOD(Framebuffer, Destroy, void, (void* handle), Yonai.Graphics)
{ if (handle) delete handle; }

ADD_MANAGED_METHOD(Framebuffer, Bind, void, (void* handle), Yonai.Graphics)
{ ((Framebuffer*)handle)->Bind(); }

ADD_MANAGED_METHOD(Framebuffer, Unind, void, (void* handle), Yonai.Graphics)
{ ((Framebuffer*)handle)->Unbind(); }

ADD_MANAGED_METHOD(Framebuffer, SetSamples, void, (void* handle, unsigned int samples), Yonai.Graphics)
{ ((Framebuffer*)handle)->SetSamples(samples); }

ADD_MANAGED_METHOD(Framebuffer, SetResolution, void, (void* handle, glm::ivec2* resolution), Yonai.Graphics)
{ ((Framebuffer*)handle)->SetResolution(*resolution); }

ADD_MANAGED_METHOD(Framebuffer, GetAttachmentCount, unsigned int, (void* handle), Yonai.Graphics)
{ return ((Framebuffer*)handle)->AttachmentCount(); }

ADD_MANAGED_METHOD(Framebuffer, GetColourAttachmentCount, unsigned int, (void* handle), Yonai.Graphics)
{ return ((Framebuffer*)handle)->ColourAttachmentCount(); }

ADD_MANAGED_METHOD(Framebuffer, CopyAttachmentTo, void, (void* handle, void* destination, unsigned int attachmentIndex), Yonai.Graphics)
{ ((Framebuffer*)handle)->CopyAttachmentTo((RenderTexture*)destination, attachmentIndex); }

ADD_MANAGED_METHOD(Framebuffer, BlitTo, void, (void* handle, void* destination, unsigned int flags), Yonai.Graphics)
{ ((Framebuffer*)handle)->BlitTo((Framebuffer*)destination); }

ADD_MANAGED_METHOD(Framebuffer, GetAttachments, MonoArray*, (void* handle, int* outDepthAttachmentIndex), Yonai.Graphics)
{
	Framebuffer* fb = (Framebuffer*)handle;
	vector<RenderTexture*> attachments = fb->GetAttachments();
	RenderTexture* depthAttachment = fb->GetDepthAttachment();
	MonoArray* output = mono_array_new(mono_domain_get(), mono_get_intptr_class(), attachments.size());
	
	// Set default value, an invalid index
	*outDepthAttachmentIndex = -1;

	// Assign handles to array
	for (size_t i = 0; i < attachments.size(); i++)
	{
		mono_array_set(output, void*, i, attachments[i]);

		// Check if this attachment is the depth attachment
		if (depthAttachment && attachments[i] == depthAttachment)
			*outDepthAttachmentIndex = (int)i;
	}

	return output;
}
#pragma endregion
