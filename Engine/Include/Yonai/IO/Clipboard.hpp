#pragma once
#include <string>

namespace Yonai::IO
{
	struct Clipboard
	{
		static void SetText(char* text);
		static void SetText(std::string text);
		static std::string GetText();
	};
}