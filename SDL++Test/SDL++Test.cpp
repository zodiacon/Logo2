// SDL++Test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <SDL3++.h>

#pragma comment(lib, "SDL3")

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR cmdLine, int show) {
	using namespace SDL3;

	SDL sdl(InitFlags::Everything);
	
	Window win("Test", 800, 600);
	Renderer renderer(win);

	float x = 800;
	for (;;) {
		Event evt;
		evt.Poll();
		if (evt.Type() == EventType::Quit)
			break;

		renderer.SetDrawColor(0, 0, 128, 255);
		renderer.Clear();
		renderer.SetDrawColor(25, 255, 0, 255);
		renderer.Line(0, 0, x, 600);
		renderer.Present();
		x -= 5;
		::Sleep(300);
	}

	return 0;
}
