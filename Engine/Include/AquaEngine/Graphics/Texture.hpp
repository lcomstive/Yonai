#pragma once
#include <string>
#include <glm/glm.hpp>
#include <AquaEngine/API.hpp>

namespace AquaEngine::Graphics
{
	class Texture
	{
		bool m_HDR;
		unsigned int m_ID;
		std::string m_Path;
		glm::ivec2 m_Resolution;
		int m_Filter = 0x2601;

		void GenerateImage();

	public:
		AquaAPI Texture();
		AquaAPI Texture(std::string path, bool hdr = false);
		AquaAPI ~Texture();

		AquaAPI bool GetHDR();
		AquaAPI unsigned int GetID();
		AquaAPI std::string GetPath();
		AquaAPI glm::ivec2& GetResolution();
		
		AquaAPI void Bind(unsigned int index = 0);

		AquaAPI void Import(const char* path, bool hdr = false, int filter = 0x2601);
		AquaAPI void Import(std::string path, bool hdr = false, int filter = 0x2601);
	};
}
