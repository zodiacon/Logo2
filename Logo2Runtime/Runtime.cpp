#include "pch.h"
#include "Runtime.h"
#include <Interpreter.h>

using namespace Logo2;

Runtime::Runtime(Interpreter& inter) {
    inter.AddNativeFunction("fd", 1, [this](auto& intr, auto& args) -> Value {
        auto& t = GetTurtle();
        t.Forward(args[0].ToFloat());
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

}

Turtle& Runtime::GetTurtle() {
	return m_Turtle;
}

