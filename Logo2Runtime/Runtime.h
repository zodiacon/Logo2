#pragma once

#include <Window.h>
#include <Renderer.h>
#include "Turtle.h"

class Runtime {
public:
	Runtime();

	Turtle& GetTurtle();

	bool PumpMessages();
	void Draw();

private:
	bool Init();

	SDL3::Window m_Window;
	SDL3::Renderer m_Render;
	Turtle m_Turtle;
};

