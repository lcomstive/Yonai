#pragma once
#include <Yonai/Application.hpp>

class BaseGame : public Yonai::WindowedApplication
{
	void InitialiseScripting();

public:
	BaseGame(int argc, char** argv);
	
protected:
	void Setup() override;
	void OnPostDraw() override;
};
