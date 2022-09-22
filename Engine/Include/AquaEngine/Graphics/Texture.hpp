#pragma once
#include <string>
#include <AquaEngine/API.hpp>

namespace AquaEngine::Graphics
{
	class Texture
	{
		unsigned int m_ID;
		std::string m_Path;

		void GenerateImage(bool hdr);

	public:
		AquaAPI Texture();
		AquaAPI Texture(std::string path, bool hdr = false);
		AquaAPI ~Texture();

		AquaAPI unsigned int GetID();
		AquaAPI std::string GetPath();
		
		AquaAPI void Bind(unsigned int index = 0);
	};
}