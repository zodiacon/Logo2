#include "pch.h"
#include "Interpreter.h"
#include <Errors.h>

using namespace Logo2;

Logo2::Interpreter::Interpreter() {
    //
    // push global scope
    //
    m_Scopes.push(std::make_unique<Scope>());
}

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
        case TokenType::GreaterThan: return expr->Left()->Accept(this) > expr->Right()->Accept(this);
        case TokenType::GreaterThanOrEqual: return expr->Left()->Accept(this) >= expr->Right()->Accept(this);
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
    auto v = FindVariable(expr->Name());
    if (v)
        return v->VarValue;
    return Value();
}

Value Interpreter::VisitBlock(BlockExpression const* expr) {
    Value result;
    //PushScope();
    for (auto expr : expr->Expressions()) {
        result = Eval(expr);
    }
    //PopScope();
    return result;
}

Value Interpreter::VisitVar(VarStatement const* expr) {
    auto value = expr->Init() ? expr->Init()->Accept(this) : Value();
    Variable var;
    var.Flags = expr->IsConst() ? VariableFlags::Const : VariableFlags::None;
    var.VarValue = std::move(value);
    AddVariable(expr->Name(), std::move(var));
    return Value();
}

Value Interpreter::VisitAssign(AssignExpression const* expr) {
    auto v = FindVariable(expr->Variable());
    if(v) {
        if ((v->Flags & VariableFlags::Const) == VariableFlags::Const) {
            //
            // cannot assign to const variable
            //
            throw RuntimeError(ErrorType::CannotAssignConst, expr);
        }
        v->VarValue = Eval(expr->Value());
        return v->VarValue;
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
        else if (f.Code) {
            //
            // bind arguments
            //
            assert(f.ArgCount == args.size());
            PushScope();
            for (int i = 0; i < f.ArgCount; i++) {
                Variable v;
                v.Flags = VariableFlags::None;
                v.VarValue = std::move(args[i]);
                AddVariable(f.Parameters[i], std::move(v));
            }
            Value result;
            try {
                result = Eval(f.Code);
            }
            catch (Return const& ret) {
                result = ret.ReturnValue->Accept(this);
            }
            PopScope();
            return result;
        }
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
        try {
            Eval(expr->Block());
        }
        catch (BreakOrContinue const& bc) {
            if (!bc.Continue)
                break;
        }
    }
    return nullptr;     // repeat has no return value
}

Value Logo2::Interpreter::VisitWhile(WhileStatement const* stmt) {
    while (Eval(stmt->Condition()).ToBoolean()) {
        try {
            Eval(stmt->Block());
        }
        catch (BreakOrContinue const& bc) {
            if (!bc.Continue)
                break;
        }
    }
    return Value();
}

Value Logo2::Interpreter::VisitIfThenElse(IfThenElseExpression const* expr) {
    if (Eval(expr->Condition()).ToBoolean())
        return Eval(expr->Then());
    return expr->Else() ? Eval(expr->Else()) : nullptr;
}

Value Logo2::Interpreter::VisitFunctionDeclaration(FunctionDeclaration const* decl) {
    Function f;
    f.ArgCount = (int)decl->Parameters().size();
    f.Code = decl->Body();
    f.Parameters = decl->Parameters();
    m_Functions.insert({ decl->Name(), std::move(f) });

    return Value();
}

Value Logo2::Interpreter::VisitReturn(ReturnStatement const* stmt) {
    throw Return(stmt->ReturnValue());
}

Value Logo2::Interpreter::VisitBreakContinue(BreakOrContinueStatement const* stmt) {
    throw BreakOrContinue(stmt->IsContinue());
}

bool Interpreter::AddNativeFunction(std::string name, int arity, NativeFunction nf) {
    Function f;
    f.ArgCount = arity;
    f.NativeCode = nf;
    return m_Functions.insert({ std::move(name), std::move(f) }).second;
}

bool Logo2::Interpreter::AddVariable(std::string name, Variable var) {
    return m_Scopes.top()->AddVariable(std::move(name), std::move(var));
}

Variable const* Logo2::Interpreter::FindVariable(std::string const& name) const {
    return m_Scopes.top()->FindVariable(name);
}

Variable* Logo2::Interpreter::FindVariable(std::string const& name) {
    return m_Scopes.top()->FindVariable(name);
}

void Logo2::Interpreter::PushScope() {
    m_Scopes.push(std::make_unique<Scope>(m_Scopes.top().get()));
}

void Logo2::Interpreter::PopScope() {
    m_Scopes.pop();
}

Logo2::Scope::Scope(Scope* parent) : m_Parent(parent) {
}

bool Logo2::Scope::AddVariable(std::string name, Variable var) {
    return m_Variables.insert({ std::move(name), std::move(var) }).second;
}

Variable const* Logo2::Scope::FindVariable(std::string const& name) const {
    if (auto it = m_Variables.find(name); it != m_Variables.end())
        return &it->second;

    return m_Parent ? m_Parent->FindVariable(name) : nullptr;
}

Variable* Logo2::Scope::FindVariable(std::string const& name) {
    if (auto it = m_Variables.find(name); it != m_Variables.end())
        return &it->second;

    return m_Parent ? m_Parent->FindVariable(name) : nullptr;
}
