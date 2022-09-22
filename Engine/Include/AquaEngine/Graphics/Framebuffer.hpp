#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <AquaEngine/ResourceID.hpp>
#include <AquaEngine/Graphics/Shader.hpp>
#include <AquaEngine/Graphics/RenderTexture.hpp>

namespace AquaEngine::Graphics
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

		AquaAPI void Bind();
		AquaAPI void Unbind();

		AquaAPI void SetSamples(unsigned int samples);
		AquaAPI void SetResolution(glm::ivec2 newResolution);
		
		AquaAPI void CopyAttachmentTo(RenderTexture* destination, unsigned int colourAttachment = 0);
		AquaAPI void BlitTo(Framebuffer* other = nullptr, GLbitfield bufferFlags = GL_COLOR_BUFFER_BIT, GLenum filter = GL_NEAREST);

		AquaAPI unsigned int GetSamples();
		AquaAPI glm::ivec2 GetResolution();
		AquaAPI RenderTexture* GetDepthAttachment();
		AquaAPI RenderTexture* GetColourAttachment(unsigned int index = 0);
		AquaAPI std::vector<RenderTexture*> GetAttachments();
		AquaAPI std::vector<RenderTexture*> GetColourAttachments();

		AquaAPI unsigned int AttachmentCount();
		AquaAPI unsigned int ColourAttachmentCount();

		AquaAPI bool HasDepthAttachment();

		AquaAPI FramebufferSpec& GetSpecs();
	};
}
