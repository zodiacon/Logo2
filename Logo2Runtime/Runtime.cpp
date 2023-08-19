#include "pch.h"
#include "Runtime.h"
#include <Interpreter.h>
#include <cassert>
#include <print>

#pragma comment(lib, "gdiplus")

using namespace Logo2;

Runtime::Runtime(Interpreter& inter) {
    inter.AddNativeFunction("fd", 1, [this](auto& intr, auto& args) -> Value {
        GetTurtle().Forward(args[0].ToFloat());
        return Value();
        });
    inter.AddNativeFunction("bk", 1, [this](auto& intr, auto& args) -> Value {
        GetTurtle().Back(args[0].ToFloat());
        return Value();
        });
    inter.AddNativeFunction("penwidth", 1, [this](auto& intr, auto& args) -> Value {
        GetTurtle().SetPenWidth(args[0].ToFloat());
        return Value();
        });
    inter.AddNativeFunction("rt", 1, [this](auto& intr, auto& args) -> Value {
        auto& t = GetTurtle();
        t.Rotate(args[0].ToFloat());
        return Value();
        });
    inter.AddNativeFunction("penup", 0, [this](auto& intr, auto& args) -> Value {
        GetTurtle().Penup();
        return Value();
        });
    inter.AddNativeFunction("pendown", 0, [this](auto& intr, auto& args) -> Value {
        GetTurtle().Pendown();
        return Value();
        });
    inter.AddNativeFunction("pencolor", 3, [this](auto& intr, auto& args) -> Value {
        GetTurtle().SetPenColor((BYTE)args[0].ToInteger(), (BYTE)args[1].ToInteger(), (BYTE)args[2].ToInteger());
        return Value();
        });
    inter.AddNativeFunction("print", 1, [this](auto& intr, auto& args) -> Value {
        std::printf(args[0].ToString().c_str());
        return Value();
        });
    inter.AddNativeFunction("println", 1, [this](auto& intr, auto& args) -> Value {
        std::printf("%s\n", args[0].ToString().c_str());
        return Value();
        });

}

Turtle& Runtime::GetTurtle() {
	return m_Turtle;
}

bool Runtime::Init() {
    if (m_Init)
        return true;

    ULONG_PTR token;
    Gdiplus::GdiplusStartupOutput out;
    Gdiplus::GdiplusStartupInput input;

    if (Gdiplus::Status::Ok != Gdiplus::GdiplusStartup(&token, &input, &out))
        return false;

    WNDCLASS wc{};
    wc.lpfnWndProc = WindowProc;
    wc.lpszClassName = L"LogoWindowClass";
    wc.hInstance = ::GetModuleHandle(nullptr);
    wc.hCursor = ::LoadCursor(nullptr, IDC_ARROW);
    wc.hIcon = ::LoadIcon(nullptr, IDI_APPLICATION);
    wc.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;
    
    return ::RegisterClass(&wc);
}

bool Runtime::CreateLogoWindow(PCWSTR text, int width, int height) {
    auto ok = m_Window.Create(text, width, height);
    if (ok) {
        ::ShowWindow(m_Window, SW_SHOW);
        ::UpdateWindow(m_Window);
        m_Window.AddTurtle(&m_Turtle);
    }
    return ok;
}

LRESULT Runtime::WindowProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_DESTROY:
            ::PostQuitMessage(0);
            break;

        case WM_CREATE:
        {
            auto cs = (LPCREATESTRUCT)lp;
            assert(cs->lpCreateParams);
            ::SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)cs->lpCreateParams);
            return 0;
        }

        case WM_PAINT:
        {
            auto win = (Window*)::GetWindowLongPtr(hWnd, GWLP_USERDATA);
            return win->DoPaint();
        }
    }
    return ::DefWindowProc(hWnd, msg, wp, lp);
}

