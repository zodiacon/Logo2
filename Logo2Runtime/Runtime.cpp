#include "pch.h"
#include "Runtime.h"
#include <SimpleTypes.h>

Runtime::Runtime() : m_Turtle(m_Render) {
	Init();
}

bool Runtime::Init() {
	SDL3::SDL::Init();
	m_Window = SDL3::Window("Logo2 Window", 800, 600);
	m_Render = SDL3::Renderer(m_Window);
	m_Turtle.SetCenter(400, 300);

	return true;
}

Turtle& Runtime::GetTurtle() {
	return m_Turtle;
}

bool Runtime::PumpMessages() {
	SDL3::Event evt;
	if (evt.Poll()) {
		if (evt.Type() == SDL3::EventType::Quit)
			return false;
	}
	return true;
}

void Runtime::Draw() {
	m_Turtle.Draw();
	m_Render.Present();
}
