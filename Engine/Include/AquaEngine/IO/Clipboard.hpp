#pragma once
#include <string>

namespace AquaEngine::IO
{
	struct Clipboard
	{
		static void SetText(char* text);
		static void SetText(std::string text);
		static std::string GetText();
	};
}