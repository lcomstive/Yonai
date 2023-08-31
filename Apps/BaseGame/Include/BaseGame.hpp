#pragma once
#include <Yonai/Application.hpp>
#include <Yonai/Systems/Global/RenderSystem.hpp>

class BaseGame : public Yonai::WindowedApplication
{
	Yonai::Systems::RenderSystem* m_RenderSystem = nullptr;
	void InitialiseScripting();

public:
	BaseGame(int argc, char** argv);
	
protected:
	void Setup() override;
	void OnPreDraw() override;
	void OnPostDraw() override;
};
