#pragma once
#include <AquaEngine/Application.hpp>
#include <AquaEngine/Systems/Global/RenderSystem.hpp>

class BaseGame : public AquaEngine::WindowedApplication
{
	AquaEngine::Systems::RenderSystem* m_RenderSystem = nullptr;
	void InitialiseScripting();

public:
	BaseGame(int argc, char** argv);
	
protected:
	void Setup() override;
	void OnPreDraw() override;
	void OnPostDraw() override;
};
