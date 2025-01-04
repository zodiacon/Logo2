#include "pch.h"
#include "Interpreter.h"
#include <Errors.h>

using namespace Logo2;

Interpreter::Interpreter() {
	//
	// push global scope
	//
	m_Scopes.push(std::make_unique<Scope>());
}

Value Interpreter::Eval(LogoAstNode const* node) {
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
	case TokenType::And: return expr->Left()->Accept(this) & expr->Right()->Accept(this);
	case TokenType::Or: return expr->Left()->Accept(this) | expr->Right()->Accept(this);
	case TokenType::Xor: return expr->Left()->Accept(this) ^ expr->Right()->Accept(this);
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
	case TokenType::Not: return !value;
	}
	throw RuntimeError(ErrorType::UndefinedOperator, expr->Arg());
}

Value Interpreter::VisitName(NameExpression const* expr) {
	auto v = FindVariable(expr->Name());
	if (v)
		return v->VarValue;
	throw RuntimeError(ErrorType::UndefinedSymbol, expr);
}

Value Interpreter::VisitBlock(BlockExpression const* expr) {
	Value result;
	//    PushScope();
	for (auto expr : expr->Expressions()) {
		result = Eval(expr);
	}
	//    PopScope();
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
	if (v) {
		if ((v->Flags & VariableFlags::Const) == VariableFlags::Const) {
			//
			// cannot assign to const variable
			//
			throw RuntimeError(ErrorType::CannotAssignConst, expr);
		}
		v->VarValue = Eval(expr->Value());
		return v->VarValue;
	}
	throw RuntimeError(ErrorType::UndefinedSymbol, expr);
}

Value Interpreter::VisitPostfix(PostfixExpression const* expr) {
	return Value();
}

Value Interpreter::InvokeFunction(Function const& f, InvokeFunctionExpression const* expr) {
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
		auto scopes = m_Scopes.size();
		if (f.Environment)
			m_Scopes.push(f.Environment->Clone(m_Scopes.top().get()));
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
		while (m_Scopes.size() > scopes)
			PopScope();
		return result;
	}
	assert(false);
	return nullptr;
}

Value Interpreter::VisitInvokeFunction(InvokeFunctionExpression const* expr) {
	if (auto it = m_Functions.find(expr->Name()); it != m_Functions.end()) {
		return InvokeFunction(it->second, expr);
	}
	auto var = FindVariable(expr->Name());
	if (var) {
		if (var->VarValue.IsFunction())
			return InvokeFunction(*(var->VarValue.Func()), expr);
		throw RuntimeError(ErrorType::NotCallable, expr);
	}
	throw RuntimeError(ErrorType::UndefinedFunction);
}

Value Interpreter::VisitRepeat(RepeatStatement const* expr) {
	auto count = Eval(expr->Count());
	if (!count.IsInteger())
		throw RuntimeError(ErrorType::TypeMismatch, expr->Count());

	auto n = count.Integer();
	PushScope();
	while (n-- > 0) {
		try {
			Eval(expr->Block());
		}
		catch (BreakOrContinue const& bc) {
			if (!bc.Continue)
				break;
		}
	}
	PopScope();
	return nullptr;     // repeat has no return value
}

Value Interpreter::VisitWhile(WhileStatement const* stmt) {
	while (Eval(stmt->Condition()).ToBoolean()) {
		PushScope();
		try {
			Eval(stmt->Body());
		}
		catch (BreakOrContinue const& bc) {
			if (!bc.Continue) {
				PopScope();
				break;
			}
		}
		PopScope();
	}
	return Value();
}

Value Interpreter::VisitIfThenElse(IfThenElseExpression const* expr) {
	Value result;
	if (Eval(expr->Condition()).ToBoolean()) {
		PushScope();
		result = Eval(expr->Then());
		PopScope();
	}
	else if (expr->Else()) {
		PushScope();
		result = Eval(expr->Else());
		PopScope();
	}
	return result;
}

Value Interpreter::VisitFunctionDeclaration(FunctionDeclaration const* decl) {
	Function f;
	f.ArgCount = (int)decl->Parameters().size();
	f.Code = decl->Body();
	f.Parameters = decl->Parameters();
	m_Functions.try_emplace(decl->Name(), std::move(f));

	return Value();
}

Value Interpreter::VisitReturn(ReturnStatement const* stmt) {
	throw Return(stmt->ReturnValue());
}

Value Interpreter::VisitBreakContinue(BreakOrContinueStatement const* stmt) {
	throw BreakOrContinue(stmt->IsContinue());
}

Value Interpreter::VisitFor(ForStatement const* stmt) {
	for (Eval(stmt->Init()); Eval(stmt->While()).ToBoolean(); Eval(stmt->Inc())) {
		try {
			Eval(stmt->Body());
		}
		catch (BreakOrContinue const& bc) {
			if (!bc.Continue)
				break;
		}
	}
	return nullptr;
}

Value Interpreter::VisitStatements(Statements const* stmts) {
	Value result;
	for (auto& stmt : stmts->Get())
		result = Eval(stmt.get());
	return result;
}

Value Interpreter::VisitAnonymousFunction(AnonymousFunctionExpression const* func) {
	auto f = std::make_shared<Function>();
	f->ArgCount = (int)func->Args().size();
	f->Code = func->Body();
	f->Parameters = func->Args();
	f->Environment = m_Scopes.top()->Clone();

	return Value(f);
}

bool Interpreter::AddNativeFunction(std::string name, int arity, NativeFunction nf) {
	Function f;
	f.ArgCount = arity;
	f.NativeCode = nf;
	return m_Functions.insert({ std::move(name), std::move(f) }).second;
}

bool Interpreter::AddVariable(std::string name, Variable var) {
	return m_Scopes.top()->AddVariable(std::move(name), std::move(var));
}

Variable const* Interpreter::FindVariable(std::string const& name) const {
	return m_Scopes.top()->FindVariable(name);
}

Variable* Interpreter::FindVariable(std::string const& name) {
	return m_Scopes.top()->FindVariable(name);
}

void Interpreter::PushScope() {
	m_Scopes.push(std::make_unique<Scope>(m_Scopes.top().get()));
}

void Interpreter::PopScope() {
	m_Scopes.pop();
}

Scope::Scope(Scope* parent) : m_Parent(parent) {
}

bool Scope::AddVariable(std::string name, Variable var) {
	return m_Variables.insert({ std::move(name), std::move(var) }).second;
}

Variable const* Scope::FindVariable(std::string const& name) const {
	if (auto it = m_Variables.find(name); it != m_Variables.end())
		return &it->second;

	return m_Parent ? m_Parent->FindVariable(name) : nullptr;
}

Variable* Scope::FindVariable(std::string const& name) {
	if (auto it = m_Variables.find(name); it != m_Variables.end())
		return &it->second;

	return m_Parent ? m_Parent->FindVariable(name) : nullptr;
}

std::unique_ptr<Scope> Logo2::Scope::Clone(Scope* parent) const {
	auto scope = std::make_unique<Scope>(*this);
	if(parent)
		scope->m_Parent = parent;
	return scope;
}
