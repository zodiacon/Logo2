#pragma once

#include "Turtle.h"

namespace Logo2 {
	class Interpreter;

	class Runtime {
	public:
		Runtime(Interpreter& inter);
		Turtle& GetTurtle();

	private:
		Turtle m_Turtle;
	};
}

