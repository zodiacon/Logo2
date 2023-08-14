#include "pch.h"
#include "Runtime.h"

Runtime::Runtime() : m_Turtle(m_Render) {
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
