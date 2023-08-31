#pragma once
#include <string>
#include <glm/glm.hpp>
#include <Yonai/API.hpp>

#ifndef GL_LINEAR
#define GL_LINEAR 0x2601
#endif

namespace Yonai::Graphics
{
	class Texture
	{
		bool m_HDR;
		int m_Filter;
		unsigned int m_ID;
		glm::ivec2 m_Resolution;

	public:
		YonaiAPI Texture();
		YonaiAPI ~Texture();

		YonaiAPI bool Upload(std::vector<unsigned char>& textureData, bool hdr = false, int filter = GL_LINEAR);

		YonaiAPI bool GetHDR();
		YonaiAPI bool IsValid();
		YonaiAPI int GetFilter();
		YonaiAPI unsigned int GetID();
		YonaiAPI glm::ivec2& GetResolution();
		
		YonaiAPI void Bind(unsigned int index = 0);
	};
}
