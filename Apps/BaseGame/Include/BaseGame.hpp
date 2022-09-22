#pragma once
#include <AquaEngine/Application.hpp>

class BaseGame : public AquaEngine::WindowedApplication
{
	void OnUpdate() override;

public:
	BaseGame(int argc, char** argv);
	~BaseGame();

	void Setup() override;
};
