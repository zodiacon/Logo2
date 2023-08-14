#pragma once

#include <Window.h>
#include <Renderer.h>
#include "Turtle.h"

class Runtime {
public:
	Runtime();
	bool Init();

	Turtle& GetTurtle();

private:
	SDL3::Window m_Window;
	SDL3::Renderer m_Render;
	Turtle m_Turtle;
};

