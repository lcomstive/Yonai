#pragma once
#include <string>
#include <glm/glm.hpp>
#include <AquaEngine/API.hpp>

#ifndef GL_LINEAR
#define GL_LINEAR 0x2601
#endif

namespace AquaEngine::Graphics
{
	class Texture
	{
		bool m_HDR;
		int m_Filter;
		unsigned int m_ID;
		glm::ivec2 m_Resolution;

	public:
		AquaAPI Texture();
		AquaAPI ~Texture();

		AquaAPI bool Upload(std::vector<unsigned char>& textureData, bool hdr = false, int filter = GL_LINEAR);

		AquaAPI bool GetHDR();
		AquaAPI bool IsValid();
		AquaAPI int GetFilter();
		AquaAPI unsigned int GetID();
		AquaAPI glm::ivec2& GetResolution();
		
		AquaAPI void Bind(unsigned int index = 0);
	};
}
