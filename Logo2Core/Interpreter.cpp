#include "pch.h"
#include "Interpreter.h"
#include <Errors.h>

using namespace Logo2;

Value Logo2::Interpreter::Eval(LogoAstNode const* node) {
    return node->Accept(this);
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
        result = Eval(expr);
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
        it->second.VarValue = Eval(expr->Value());
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
        if (f.ArgCount != expr->Arguments().size())
            throw RuntimeError(ErrorType::ArgumentCountMismatch, expr);

        std::vector<Value> args;
        for (auto& arg : expr->Arguments()) {
            args.push_back(arg->Accept(this));
        }
        if (f.NativeCode)
            return f.NativeCode(*this, args);
        else if (f.Code)
            return Eval(f.Code.get());
        assert(false);
    }

    throw RuntimeError(ErrorType::UndefinedFunction);
}

Value Interpreter::VisitRepeat(RepeatStatement const* expr) {
    auto count = Eval(expr->Count());
    if (!count.IsInteger())
        throw RuntimeError(ErrorType::TypeMismatch, expr->Count());

    auto n = count.Integer();
    while (n-- > 0) {
        Eval(expr->Block());
    }
    return nullptr;     // repeat has no return value
}

Value Logo2::Interpreter::VisitWhile(WhileStatement const* stmt) {
    while (Eval(stmt->Condition()).Boolean()) {
        Eval(stmt->Block());
    }
    return Value();
}

bool Interpreter::AddNativeFunction(std::string name, int arity, NativeFunction nf) {
    Function f;
    f.ArgCount = arity;
    f.NativeCode = nf;
    return m_Functions.insert({ std::move(name), std::move(f) }).second;
}


