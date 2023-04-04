#pragma once
#include <string>
#include <glm/glm.hpp>
#include <AquaEngine/API.hpp>

namespace AquaEngine::Graphics
{
	class Texture
	{
		unsigned int m_ID;
		std::string m_Path;
		glm::ivec2 m_Resolution;

		void GenerateImage(bool hdr);

	public:
		AquaAPI Texture();
		AquaAPI Texture(std::string path, bool hdr = false);
		AquaAPI ~Texture();

		AquaAPI unsigned int GetID();
		AquaAPI std::string GetPath();
		AquaAPI glm::ivec2& GetResolution();
		
		AquaAPI void Bind(unsigned int index = 0);
	};
}