#pragma once
#include <AquaEngine/Application.hpp>

class BaseGame : public AquaEngine::WindowedApplication
{
protected:
	void OnUpdate() override;
	void OnDraw() override;

	void Setup() override;
	void Cleanup() override;
};
