#include "pch.h"
#include "Interpreter.h"
#include <Errors.h>

Interpreter::Interpreter(Runtime& rt) : m_Runtime(rt) {
    Function f;
    f.ArgCount = 1;
    f.NativeCode = [](auto& intr, auto& args) -> Value {
        auto& t = intr.GetRuntime().GetTurtle();
        t.Forward(args[0].ToFloat());
        return Value();
        };
    m_Functions.insert({ "fd", std::move(f) });
    f.NativeCode = [](auto& intr, auto& args) -> Value {
        auto& t = intr.GetRuntime().GetTurtle();
        t.Rotate(args[0].ToFloat());
        return Value();
        };
    m_Functions.insert({ "rt", std::move(f) });
}


Value Interpreter::VisitLiteral(LiteralExpression const* expr) {
    auto& lit = expr->Literal();
    switch (lit.Type) {
        case TokenType::Integer:
            return std::stoll(lit.Lexeme, nullptr, 0);

        case TokenType::Real:
            return std::stod(lit.Lexeme);

        case TokenType::String:
            return lit.Lexeme;

        case TokenType::Keyword_True:
            return true;

        case TokenType::Keyword_False:
            return false;
    }
    return nullptr;
}

Value Interpreter::VisitBinary(BinaryExpression const* expr) {
    switch (expr->Operator().Type) {
        case TokenType::Add: return expr->Left()->Accept(this) + expr->Right()->Accept(this);
        case TokenType::Sub: return expr->Left()->Accept(this) - expr->Right()->Accept(this);
        case TokenType::Mul: return expr->Left()->Accept(this) * expr->Right()->Accept(this);
        case TokenType::Div: return expr->Left()->Accept(this) / expr->Right()->Accept(this);
        case TokenType::Mod: return expr->Left()->Accept(this) % expr->Right()->Accept(this);
        case TokenType::Equal: return expr->Left()->Accept(this) == expr->Right()->Accept(this);
        case TokenType::NotEqual: return expr->Left()->Accept(this) != expr->Right()->Accept(this);
        case TokenType::LessThan: return expr->Left()->Accept(this) < expr->Right()->Accept(this);
        case TokenType::LessThanOrEqual: return expr->Left()->Accept(this) <= expr->Right()->Accept(this);
    }
    return Value();
}

Value Interpreter::VisitUnary(UnaryExpression const* expr) {
    auto value = expr->Arg()->Accept(this);
    switch (expr->Operator().Type) {
        case TokenType::Sub: return -value;
        case TokenType::Add: return value;
    }
    return Value();
}

Value Interpreter::VisitName(NameExpression const* expr) {
    if (auto it = m_Variables.find(expr->Name()); it != m_Variables.end())
        return it->second.VarValue;
    return Value();
}

Value Interpreter::VisitBlock(BlockExpression const* expr) {
    Value result;
    for (auto expr : expr->Expressions()) {
        result = expr->Accept(this);
    }
    return result;
}

Value Interpreter::VisitVar(VarStatement const* expr) {
    auto value = expr->Init() ? expr->Init()->Accept(this) : Value();
    Variable var;
    var.Flags = expr->IsConst() ? VariableFlags::Const : VariableFlags::None;
    var.VarValue = std::move(value);
    m_Variables.insert({ expr->Name(), std::move(var) });
    return Value();
}

Value Interpreter::VisitAssign(AssignExpression const* expr) {
    if (auto it = m_Variables.find(expr->Variable()); it != m_Variables.end()) {
        if ((it->second.Flags & VariableFlags::Const) == VariableFlags::Const) {
            //
            // cannot assign to const variable
            //
            throw RuntimeError(ErrorType::CannotAssignConst, expr);
        }
        it->second.VarValue = expr->Value()->Accept(this);
        return it->second.VarValue;
    }
    assert(false);
    return Value();
}

Value Interpreter::VisitPostfix(PostfixExpression const* expr) {
    return Value();
}

Value Interpreter::VisitInvokeFunction(InvokeFunctionExpression const* expr) {
    if (auto it = m_Functions.find(expr->Name()); it != m_Functions.end()) {
        auto& f = it->second;
        std::vector<Value> args;
        for (auto& arg : expr->Arguments()) {
            args.push_back(arg->Accept(this));
        }
        if (f.NativeCode)
            return f.NativeCode(*this, args);
    }

    return Value();
}

Value Interpreter::VisitRepeat(RepeatStatement const* expr) {
    auto count = expr->Count()->Accept(this);
    if (!count.IsInteger())
        throw RuntimeError(ErrorType::TypeMismatch, expr->Count());

    auto n = count.Integer();
    while (n-- > 0) {
        expr->Block()->Accept(this);
    }
    return nullptr;     // repeat has no return value
}

Runtime& Interpreter::GetRuntime() {
    return m_Runtime;
}

