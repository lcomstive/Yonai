#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <Yonai/ResourceID.hpp>
#include <Yonai/Graphics/Shader.hpp>
#include <Yonai/Graphics/RenderTexture.hpp>

namespace Yonai::Graphics
{
	struct FramebufferSpec
	{
		glm::ivec2 Resolution = { 0, 0 };

		/// <summary>
		/// When set to true, framebuffer target is the screen
		/// </summary>
		bool SwapchainTarget = false;

		/// <summary>
		/// Multisampling, or disable by setting to 1
		/// </summary>
		unsigned int Samples = 1;
		
		std::vector<TextureFormat> Attachments;
	};

	class Framebuffer
	{
		unsigned int m_ID;
		FramebufferSpec m_Specs;

		RenderTexture* m_DepthAttachment;
		std::vector<RenderTexture*> m_ColourAttachments;

		void Destroy();
		void Create();
		void Recreate(); // Usually called when resized or samples change, attachment count & formats remain constant

	public:
		Framebuffer(FramebufferSpec& specs);
		~Framebuffer();

		YonaiAPI void Bind();
		YonaiAPI void Unbind();

		YonaiAPI void SetSamples(unsigned int samples);
		YonaiAPI void SetResolution(glm::ivec2 newResolution);
		
		YonaiAPI void CopyAttachmentTo(RenderTexture* destination, unsigned int colourAttachment = 0);
		YonaiAPI void BlitTo(Framebuffer* other = nullptr);

		YonaiAPI unsigned int GetSamples();
		YonaiAPI glm::ivec2 GetResolution();
		YonaiAPI RenderTexture* GetDepthAttachment();
		YonaiAPI RenderTexture* GetColourAttachment(unsigned int index = 0);
		YonaiAPI std::vector<RenderTexture*> GetAttachments();
		YonaiAPI std::vector<RenderTexture*> GetColourAttachments();

		YonaiAPI unsigned int AttachmentCount();
		YonaiAPI unsigned int ColourAttachmentCount();

		YonaiAPI bool HasDepthAttachment();

		YonaiAPI FramebufferSpec& GetSpecs();
	};
}
