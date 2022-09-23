#pragma once
#include <AquaEngine/Application.hpp>

class BaseGame : public AquaEngine::WindowedApplication
{
protected:
	void OnUpdate() override;
	void OnDraw() override;

public:
	BaseGame(int argc, char** argv);
	~BaseGame();

	void Setup() override;
};
