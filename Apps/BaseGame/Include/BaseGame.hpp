#pragma once
#include <AquaEngine/Application.hpp>
#include <AquaEngine/Systems/Global/RenderSystem.hpp>

class BaseGame : public AquaEngine::WindowedApplication
{
	void InitialiseScripting();

public:
	BaseGame(int argc, char** argv);
	
protected:
	void OnPostDraw() override;
};
