#pragma once

#include "Turtle.h"
#include "Window.h"

namespace Logo2 {
	class Interpreter;

	class Runtime {
	public:
		Runtime(Interpreter& inter);
		bool Init();
		bool CreateLogoWindow(PCWSTR text, int width, int height);

		Turtle& GetTurtle();

	private:
		static LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);

		Turtle m_Turtle;
		Window m_Window;

		inline static bool m_Init{ false };
	};
}

