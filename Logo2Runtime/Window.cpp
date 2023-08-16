#include "pch.h"
#include "Window.h"
#include "Turtle.h"

using namespace Logo2;
using namespace Gdiplus;

HWND Window::Create(PCWSTR text, int width, int height, DWORD style) {
    RECT rc{ 0, 0, width, height };
    ::AdjustWindowRect(&rc, style, FALSE);
    m_hWnd = ::CreateWindow(L"LogoWindowClass", text, style, CW_USEDEFAULT, CW_USEDEFAULT,
        rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, ::GetModuleHandle(nullptr), this);
    return m_hWnd;
}

void Logo2::Window::AddTurtle(Turtle* t) {
    m_Turtles.push_back(t);
    t->SetNotify(this);
    ::InvalidateRect(m_hWnd, nullptr, TRUE);
}

LRESULT Window::DoPaint() {
    PAINTSTRUCT ps;
    auto hdc = ::BeginPaint(m_hWnd, &ps);
    Graphics g(hdc);
    RECT rc;
    ::GetClientRect(m_hWnd, &rc);
    g.ScaleTransform(1, -1);
    g.TranslateTransform(rc.right / 2.0f, -rc.bottom / 2.0f);
    m_Width = rc.right;
    m_Height = rc.bottom;
    Draw(g);
    ::EndPaint(m_hWnd, &ps);
    return 0;
}

void Window::Draw(Gdiplus::Graphics& g) const {
    g.Clear(Color::WhiteSmoke);
    m_Pen.reset(new Pen(Color::Black));

    for (auto t : m_Turtles)
        DrawTurtle(g, t);
}

Window::operator HWND() const {
    return m_hWnd;
}

void Logo2::Window::DrawTurtle(Gdiplus::Graphics& g, Turtle* t) const {
    for (auto& cmd : t->GetCommands()) {
        DrawTurtleCommand(g, t, cmd);
    }
}

void Logo2::Window::DrawTurtleCommand(Gdiplus::Graphics& g, Turtle* t, TurtleCommand const& cmd) const {
    switch (cmd.Type) {
        case TurtleCommandType::DrawLine:
            g.DrawLine(m_Pen.get(), cmd.Line.From.X, cmd.Line.From.Y, cmd.Line.To.X, cmd.Line.To.Y);
            break;

        case TurtleCommandType::SetWidth:
        {
            Color color;
            m_Pen->GetColor(&color);
            m_Pen.reset(new Pen(color, cmd.Width));
            break;
        }

        case TurtleCommandType::SetColor:
        {
            Color color;
            color.SetValue(cmd.Color);
            m_Pen.reset(new Pen(color, m_Pen->GetWidth()));
            break;
        }
    }
}

void Logo2::Window::AddCommand(Turtle* turtle, TurtleCommand const& cmd) {
    Graphics g(m_hWnd);
    g.ScaleTransform(1, -1);
    g.TranslateTransform(m_Width / 2.0f, -m_Height / 2.0f);
    DrawTurtleCommand(g, turtle, cmd);
}
