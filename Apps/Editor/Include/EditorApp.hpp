#pragma once
#include <string>
#include <AquaEngine/Application.hpp>

namespace AquaEditor
{
	class EditorApp : public AquaEngine::WindowedApplication
	{
		std::string m_ProjectPath = "";

	protected:
		void Setup() override;
	};
}