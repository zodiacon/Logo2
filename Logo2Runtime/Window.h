#pragma once

#include "Turtle.h"

namespace Logo2 {
	class Window : ICommandNotify {
	public:
		HWND Create(PCWSTR text, int width, int height, DWORD style = WS_OVERLAPPED | WS_BORDER | WS_CAPTION | WS_SYSMENU);
		void AddTurtle(Turtle* t);

		LRESULT DoPaint();
		void Draw(Gdiplus::Graphics& g) const;
		operator HWND() const;
		void DrawTurtle(Gdiplus::Graphics& g, Turtle* t) const;
		void DrawTurtleCommand(Gdiplus::Graphics& g, Turtle* t, TurtleCommand const& cmd) const;

		// Inherited via ICommandNotify
		void AddCommand(Turtle* turtle, TurtleCommand const& cmd) override;

	private:
		int m_Width, m_Height;
		HWND m_hWnd;
		std::vector<Turtle*> m_Turtles;
		mutable std::unique_ptr<Gdiplus::Pen> m_Pen;
	};
}

